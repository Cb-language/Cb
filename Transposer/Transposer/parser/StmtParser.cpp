#include "StmtParser.h"

#include "TypeParser.h"
#include "ExprParser.h"

#include "class/AccessType.h"


#include "AST/statements/VarDeclStmt.h"
#include "AST/statements/AssignmentStmt.h"
#include "AST/statements/HearStmt.h"
#include "AST/statements/PlayStmt.h"
#include "AST/statements/BodyStmt.h"
#include "AST/statements/FuncDeclStmt.h"
#include "AST/statements/ReturnStmt.h"
#include "AST/statements/FuncCreditStmt.h"
#include "AST/statements/IfStmt.h"
#include "AST/statements/ArrayDeclStmt.h"
#include "AST/statements/WhileStmt.h"
#include "AST/statements/BreakStmt.h"
#include "AST/statements/CaseStmt.h"
#include "AST/statements/SwitchStmt.h"
#include "AST/statements/ContinueStmt.h"
#include "AST/statements/ForStmt.h"
#include "AST/statements/ClassDeclStmt.h"
#include "AST/statements/ConstractorDeclStmt.h"
#include "AST/statements/ConstractorCallStmt.h"
#include "AST/statements/ObjCreationStmt.h"


#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/MissingSemicolon.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingParenthesis.h"
#include "errorHandling/syntaxErrors/ExpectedKeywordNotFound.h"
#include "errorHandling/syntaxErrors/IllegalVarName.h"

#include "errorHandling/semanticErrors/IdentifierTaken.h"
#include "errorHandling/semanticErrors/StmtNotBreakable.h"
#include "errorHandling/semanticErrors/StmtNotContinueAble.h"
#include "errorHandling/semanticErrors/UnrecognizedIdentifier.h"

#include "errorHandling/entryPointErrors/MainOverride.h"

#include "errorHandling/classErrors/NoCtor.h"
#include "errorHandling/classErrors/InvalidAccessKeyword.h"
#include "errorHandling/classErrors/MissingClassPipe.h"
#include "errorHandling/classErrors/NoOverrideError.h"
#include "errorHandling/how/HowDareYou.h"
#include "errorHandling/syntaxErrors/NoLineOpener.h"
#include "errorHandling/syntaxErrors/NoNewLine.h"

StmtParser::StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser)
    : c(c), typeParser(typeParser), exprParser(exprParser)
{
}

bool StmtParser::expectSemiColon() const
{
    return c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current()));
}

void StmtParser::parse()
{
    while (!c.isAtEnd())
    {
        if (auto stmt = parseStmt(true))
        {
            c.getStmts().push_back(std::move(stmt));
        }
        else
        {
            c.advance();
        }
    }
}

std::unique_ptr<Stmt> StmtParser::parseStmt(const bool isGlobal, const bool isBreakable, const bool isContinueAble)
{
    if (c.matchNonConsume(TokenType::KEYWORD_C_CLEF))
    {
        throw HowDareYou(c.current());
    }

    if (c.matchNonConsume(TokenType::TYPE_RIFF))
    {
        return parseArrayDeclStmt();
    }
    if (c.isType())
    {
        return parseVarDecStmt();
    }
    if (c.matchNonConsume(TokenType::IDENTIFIER))
    {
        if (c.peek().type == TokenType::IDENTIFIER)
        {
            return parseObjCreationStmt();
        }
        // Check if it's obj creation or something else

        if (c.matchNonConsume(TokenType::IDENTIFIER) && c.isUnaryOpStmtAhead())
        {
            auto expr = exprParser.parseUnaryOpExpr(true);
            return std::unique_ptr<Stmt>(dynamic_cast<Stmt*>(expr.release()));
        }
        if (c.matchNonConsume(TokenType::IDENTIFIER) && c.isAssignmentStmtAhead())
        {
            return parseAssignmentStmt();
        }
    }

    if (c.matchNonConsume(TokenType::KEYWORD_HEAR))
    {
        return parseHearStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_PLAY) || c.matchNonConsume(TokenType::KEYWORD_PLAYBAR))
    {
        return parsePlayStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_SONG))
    {
        return parseFuncDeclStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_RETURN))
    {
        return parseReturnStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_IF))
    {
        return parseIfStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_WHILE))
    {
        return parseWhileStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_SWITCH))
    {
        return parseSwitchStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_PAUSE))
    {
        return parseBreakStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_RESUME))
    {
        return parseContinueStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_FMAJ) || c.matchNonConsume(TokenType::KEYWORD_FMIN))
    {
        return parseForStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_INSTRUMENT))
    {
        return parseClassDeclStmt();
    }

    if (c.matchNonConsume(TokenType::IDENTIFIER))
    {
        // Might be a function call as a statement
        if (c.peek().type == TokenType::PUNCTUATION_PARENTHESIS_OPEN)
        {
            auto expr = exprParser.parseFuncCallExpr(true);
            return std::unique_ptr<Stmt>(dynamic_cast<Stmt*>(expr.release()));
        }
    }

    c.addError(std::make_unique<UnexpectedToken>(c.current()));
    return nullptr;
}

std::unique_ptr<VarDeclStmt> StmtParser::parseVarDecStmt(const bool isField) const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    if (!type) return nullptr;

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;
    const std::string name = nameToken.value.value();

    std::unique_ptr<Expr> init = nullptr;
    if (c.matchNonConsume(TokenType::ASSIGNMENT_OP_EQUAL))
    {
        c.advance();
        init = exprParser.parseExpr();
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    auto stmt = std::make_unique<VarDeclStmt>(
        t,
        c.getFuncDecl(),
        init != nullptr,
        std::move(init),
        Var(std::move(type), name),
        c.getClassDecl()
    );

    return stmt;
}

std::unique_ptr<AssignmentStmt> StmtParser::parseAssignmentStmt() const
{
    const Token& t = c.current();
    auto left = exprParser.parseExpr(true);
    if (!left) return nullptr;

    const Token opToken = c.current();
    if (!c.isAssignmentOp())
    {
        c.addError(std::make_unique<UnexpectedToken>(c.current()));
        return nullptr;
    }
    c.advance();

    auto right = exprParser.parseExpr();
    if (!right) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<AssignmentStmt>(
        t,
        c.getFuncDecl(),
        std::unique_ptr<Call>(dynamic_cast<Call*>(left.release())),
        opToken.value.value_or("="),
        std::move(right),
        c.getClassDecl()
    );
}

std::unique_ptr<HearStmt> StmtParser::parseHearStmt() const
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto var = exprParser.parseCallExpr();
    if (!var) return nullptr;

    std::vector<std::unique_ptr<Call>> calls;
    calls.push_back(std::move(var));

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<HearStmt>(
        t,
        c.getFuncDecl(),
        calls,
        c.getClassDecl()
    );
}

std::unique_ptr<PlayStmt> StmtParser::parsePlayStmt() const
{
    const Token& t = c.current();
    bool isBar = c.matchNonConsume(TokenType::KEYWORD_PLAYBAR);
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto expr = exprParser.parseExpr();
    if (!expr) return nullptr;

    std::vector<std::unique_ptr<Expr>> exprs;
    exprs.push_back(std::move(expr));

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<PlayStmt>(
        t,
        c.getFuncDecl(),
        std::move(exprs),
        isBar,
        c.getClassDecl()
    );
}

std::unique_ptr<BodyStmt> StmtParser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
{
    const Token& t = c.current();
    if (hasBrace)
    {
        if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    while (!c.isAtEnd() && (!hasBrace || !c.matchNonConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET)))
    {
        if (c.matchNonConsume(TokenType::PUNCTUATION_REST))
        {
            c.advance();
            if (c.matchNonConsume(TokenType::PUNCTUATION_SEMICOLON) || c.matchNonConsume(TokenType::PUNCTUATION_CLOSE_FILE)) c.advance();
            continue;
        }

        if (auto stmt = parseStmt(isGlobal, isBreakable, isContinueAble)) bodyStmts.push_back(std::move(stmt));
        
        if (!hasBrace && isGlobal) break; // In global scope we parse one by one in parse()
    }

    if (hasBrace)
    {
        if (!c.expect(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;
    }

    return std::make_unique<BodyStmt>(
        t,
        c.getFuncDecl(),
        bodyStmts,
        isGlobal,
        c.getClassDecl()
    );
}

std::unique_ptr<FuncDeclStmt> StmtParser::parseFuncDeclStmt(const bool isMethod)
{
    const Token& t = c.current();
    c.advance();

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;
    std::string name = nameToken.value.value_or("");

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    std::vector<Var> args;
    std::vector<std::pair<Var, const Token>> argsWithTokens;
    while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        if (!type) return nullptr;

        Token argNameToken;
        if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), argNameToken)) return nullptr;

        args.emplace_back(std::move(type), argNameToken.value.value_or(""));
        argsWithTokens.emplace_back(args.back(), argNameToken);

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;
        }
    }
    c.advance();

    std::unique_ptr<IType> retType = nullptr;
    if (c.matchNonConsume(TokenType::PUNCTUATION_RET_TYPE_ARROW))
    {
        c.advance();
        retType = typeParser.parseIType();
    }
    else
    {
        retType = std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA);
    }

    // Handle copyright/credits if any?
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    if (c.matchConsume(TokenType::PUNCTUATION_COPYRIGHT))
    {
        if (c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
        {
            while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
            {
                if (auto credit = parseFuncCreditStmt()) credited.push_back(std::move(credit));
                if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()));
            }
            c.advance();
        }
    }

    auto funcStmt = std::make_unique<FuncDeclStmt>(
        t,
        name,
        std::move(retType),
        args,
        credited,
        isMethod,
        VirtualType::NONE,
        false,
        c.getClassDecl()
    );

    funcStmt->setBody(parseBodyStmt(argsWithTokens, false, false, false, true));

    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt() const
{
    const Token& t = c.current();
    c.advance();

    c.expect(TokenType::PUNCTUATION_BACKSLASH, std::make_unique<UnexpectedToken>(c.current()));
    auto expr = exprParser.parseExpr();

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<ReturnStmt>(
        t,
        c.getFuncDecl(),
        expr,
        nullptr, // rType
        c.getClassDecl()
    );
}

std::unique_ptr<FuncCreditStmt> StmtParser::parseFuncCreditStmt() const
{
    const Token& t = c.current();
    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;

    return std::make_unique<FuncCreditStmt>(
        t,
        c.getFuncDecl(),
        FuncCredit(nameToken.value.value(), t),
        c.getClassDecl()
    );
}

std::unique_ptr<IfStmt> StmtParser::parseIfStmt()
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
    auto cond = exprParser.parseExpr();
    if (!cond) return nullptr;
    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto thenBody = parseBodyStmt({}, false, false, false, true);

    std::unique_ptr<Stmt> elseBody = nullptr;
    if (c.matchNonConsume(TokenType::KEYWORD_ELSE))
    {
        c.advance();
        if (c.matchNonConsume(TokenType::KEYWORD_IF)) elseBody = parseIfStmt();
        else elseBody = parseBodyStmt({}, false, false, false, true);
    }

    return std::make_unique<IfStmt>(
        t,
        c.getFuncDecl(),
        std::move(cond),
        std::move(thenBody),
        std::move(elseBody),
        false, // isElseIf
        c.getClassDecl()
    );
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt() const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    if (!type) return nullptr;

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;

    std::vector<std::unique_ptr<Expr>> sizes;
    while (c.matchNonConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        c.advance();
        sizes.push_back(exprParser.parseExpr());
        c.expect(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE);
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<ArrayDeclStmt>(
        t,
        c.getFuncDecl(),
        false, // hasStartingValue
        nullptr, // startingValue
        Var(std::move(type), nameToken.value.value_or("")),
        std::move(sizes),
        c.getClassDecl()
    );
}

std::unique_ptr<WhileStmt> StmtParser::parseWhileStmt()
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
    auto cond = exprParser.parseExpr();
    if (!cond) return nullptr;
    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto body = parseBodyStmt({}, false, true, true, true);

    return std::make_unique<WhileStmt>(
        t,
        c.getFuncDecl(),
        std::move(cond),
        std::move(body),
        c.getClassDecl()
    );
}

std::unique_ptr<BreakStmt> StmtParser::parseBreakStmt() const
{
    const Token& t = c.current();
    c.advance();
    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    return std::make_unique<BreakStmt>(t,
        c.getFuncDecl(), c.getClassDecl());
}

std::unique_ptr<CaseStmt> StmtParser::parseCaseStmt() const
{
    const Token& t = c.current();
    c.advance();

    auto val = exprParser.parseExpr();
    if (!val) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_COLON, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    return std::make_unique<CaseStmt>(
        t,
        c.getFuncDecl(),
        std::move(val),
        nullptr, // body
        false, // isDefault
        c.getClassDecl()
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt() const
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
    if (const auto expr = exprParser.parseExpr(); !expr) return nullptr;
    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.matchNonConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.matchNonConsume(TokenType::KEYWORD_CASE))
        {
            if (auto cs = parseCaseStmt()) cases.push_back(std::move(cs));
        }
        else c.advance(); // ignore for now
    }
    c.advance();

    return std::make_unique<SwitchStmt>(
        t,
        c.getFuncDecl(),
        Var(nullptr, ""), // dummy var
        cases,
        c.getClassDecl()
    );
}

std::unique_ptr<ContinueStmt> StmtParser::parseContinueStmt() const
{
    const Token& t = c.current();
    c.advance();
    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    return std::make_unique<ContinueStmt>(t,
        c.getFuncDecl(), c.getClassDecl());
}

std::unique_ptr<ForStmt> StmtParser::parseForStmt()
{
    const Token& t = c.current();
    bool isUp = c.matchNonConsume(TokenType::KEYWORD_FMAJ);
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto type = typeParser.parseIType();
    Token varToken;
    c.expect(TokenType::IDENTIFIER, nullptr, varToken);

    c.expect(TokenType::ASSIGNMENT_OP_EQUAL);
    auto start = exprParser.parseExpr();

    c.expect(TokenType::PUNCTUATION_COLON);
    auto stop = exprParser.parseExpr();

    std::unique_ptr<Expr> step = nullptr;
    if (c.matchNonConsume(TokenType::PUNCTUATION_COLON))
    {
        c.advance();
        step = exprParser.parseExpr();
    }

    c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE);

    auto body = parseBodyStmt({}, false, true, true, true);

    return std::make_unique<ForStmt>(
        t,
        c.getFuncDecl(),
        std::move(body),
        isUp,
        std::move(start),
        std::move(step),
        std::move(stop),
        varToken.value.value_or(""),
        c.getClassDecl()
    );
}

std::unique_ptr<ClassDeclStmt> StmtParser::parseClassDeclStmt()
{
    const Token& t = c.current();
    c.advance();

    Token nameToken;
    c.expect(TokenType::IDENTIFIER, nullptr, nameToken);
    std::string className = nameToken.value.value_or("");

    std::string parentName = "";
    if (c.matchNonConsume(TokenType::PUNCTUATION_COLON))
    {
        c.advance();
        Token pToken;
        c.expect(TokenType::IDENTIFIER, nullptr, pToken);
        parentName = pToken.value.value_or("");
    }

    c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET);

    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    while (!c.matchNonConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        AccessType access = AccessType::PUBLIC;
        if (c.matchConsume(TokenType::KEYWORD_PLAYERSCORE)) access = AccessType::PUBLIC;
        else if (c.matchConsume(TokenType::KEYWORD_CONDUCTORSCORE)) access = AccessType::PRIVATE;
        else if (c.matchConsume(TokenType::KEYWORD_SECTIONSCORE)) access = AccessType::PROTECTED;

        if (c.matchNonConsume(TokenType::KEYWORD_BASS))
        {
            if (auto ctor = parseCtor()) ctors.emplace_back(access, std::move(ctor));
        }
        else if (c.matchNonConsume(TokenType::KEYWORD_SONG))
        {
            if (auto method = parseFuncDeclStmt(true)) methods.emplace_back(access, std::move(method));
        }
        else if (c.isType())
        {
            if (auto field = parseVarDecStmt(true)) fields.emplace_back(access, std::move(field));
        }
        else
        {
            c.advance();
        }
    }
    c.advance();

    return std::make_unique<ClassDeclStmt>(
        t,
        c.getFuncDecl(),
        className,
        fields,
        methods,
        ctors,
        !parentName.empty(),
        "tutti",
        parentName,
        c.getClassDecl()
    );
}

std::unique_ptr<ConstractorDeclStmt> StmtParser::parseCtor()
{
    const Token& t = c.current();
    c.advance();

    c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN);
    std::vector<Var> args;
    std::vector<std::pair<Var, const Token>> argsWithTokens;
    while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        Token n;
        c.expect(TokenType::IDENTIFIER, nullptr, n);
        args.emplace_back(std::move(type), n.value.value_or(""));
        argsWithTokens.emplace_back(args.back(), n);
        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
    }
    c.advance();

    auto stmt = std::make_unique<ConstractorDeclStmt>(
        t,
        "", // className will be set by ClassDeclStmt or semantic
        args,
        c.getClassDecl()
    );

    stmt->setBody(parseBodyStmt(argsWithTokens, false, false, false, true));
    return stmt;
}

std::unique_ptr<ConstractorCallStmt> StmtParser::parseConstractorCallStmt() const
{
    return nullptr;
    // TODO implement
}

std::unique_ptr<ObjCreationStmt> StmtParser::parseObjCreationStmt() const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    Token name;
    c.expect(TokenType::IDENTIFIER, nullptr, name);

    std::vector<std::unique_ptr<Expr>> args;
    if (c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        c.advance();
        while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            args.push_back(exprParser.parseExpr());
            if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
        }
        c.advance();
    }
    
    c.expect(TokenType::PUNCTUATION_SEMICOLON);
    
    auto ctorCall = std::make_unique<ConstractorCallStmt>(t, c.getFuncDecl(), std::move(args), c.getClassDecl());

    return std::make_unique<ObjCreationStmt>(
        t,
        c.getFuncDecl(),
        nullptr, // classNode
        true,
        std::move(ctorCall),
        Var(std::move(type), name.value.value_or("")),
        c.getClassDecl()
    );
}

bool StmtParser::parseFields(std::vector<Field>& fields) { return true; }
bool StmtParser::parseMethods(std::vector<Method>& methods) { return true; }
bool StmtParser::parseCtors(std::vector<Ctor>& ctors) { return true; }
