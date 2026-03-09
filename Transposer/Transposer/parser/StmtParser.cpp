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
    if (c.getPos() == c.ge)
    {
        return c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<>());
    }
    return c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current()));
}

void StmtParser::parse()
{
    while (!c.isAtEnd())
    {
        auto stmt = parseStmt(true);
        if (stmt)
        {
            c.getStmts().push_back(std::move(stmt));
        }
        else
        {
            c.synchronize();
        }
    }
}

std::unique_ptr<Stmt> StmtParser::parseStmt(const bool isGlobal, const bool isBreakable, const bool isContinueAble)
{
    if (c.match(TokenType::PUNCTUATION_NEW_LINE))
    {
        c.advance();

        if (!isGlobal)
            c.expect(TokenType::PUNCTUATION_OPEN_LINE, std::make_unique<NoLineOpener>(c.current()));
    }

    if (c.match(TokenType::KEYWORD_C_CLEF))
    {
        throw HowDareYou(c.current());
    }

    if (c.match(TokenType::PUNCTUATION_REST))
    {
        while (c.match(TokenType::PUNCTUATION_REST))
        {
            c.advance();
            c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current()));
            c.expect(TokenType::PUNCTUATION_NEW_LINE, std::make_unique<NoNewLine>(c.current()));
            c.expect(TokenType::PUNCTUATION_OPEN_LINE, std::make_unique<NoLineOpener>(c.current()));
        }
    }

    if (c.match(TokenType::TYPE_RIFF))
    {
        return parseArrayDeclStmt();
    }
    if (c.isType())
    {
        return parseVarDecStmt();
    }
    if (c.match(TokenType::IDENTIFIER) && c.getSymTable().isClass(c.current().value.value_or("")))
    {
        // Check if it's obj creation or something else
        return parseObjCreationStmt();
    }
    if (c.match(TokenType::IDENTIFIER) && c.isUnaryOpStmtAhead())
    {
        auto expr = exprParser.parseUnaryOpExpr(true);
        return std::unique_ptr<Stmt>(dynamic_cast<Stmt*>(expr.release()));
    }
    if (c.match(TokenType::IDENTIFIER) && c.isAssignmentStmtAhead())
    {
        return parseAssignmentStmt();
    }
    if (c.match(TokenType::KEYWORD_HEAR))
    {
        return parseHearStmt();
    }
    if (c.match(TokenType::KEYWORD_PLAY) || c.match(TokenType::KEYWORD_PLAYBAR))
    {
        return parsePlayStmt();
    }
    if (c.match(TokenType::KEYWORD_SONG))
    {
        return parseFuncDeclStmt();
    }
    if (c.match(TokenType::KEYWORD_RETURN))
    {
        return parseReturnStmt();
    }
    if (c.match(TokenType::KEYWORD_IF))
    {
        return parseIfStmt();
    }
    if (c.match(TokenType::KEYWORD_WHILE))
    {
        return parseWhileStmt();
    }
    if (c.match(TokenType::KEYWORD_SWITCH))
    {
        return parseSwitchStmt();
    }
    if (c.match(TokenType::KEYWORD_PAUSE))
    {
        if (!c.getSymTable().getCurrScope()->getIsBreakable())
        {
            c.addError(std::make_unique<StmtNotBreakable>(c.current()));
            return nullptr;
        }
        return parseBreakStmt();
    }
    if (c.match(TokenType::KEYWORD_RESUME))
    {
        if (!c.getSymTable().getCurrScope()->getIsContinueAble())
        {
            c.addError(std::make_unique<StmtNotContinueAble>(c.current()));
            return nullptr;
        }
        return parseContinueStmt();
    }
    if (c.match(TokenType::KEYWORD_FMAJ) || c.match(TokenType::KEYWORD_FMIN))
    {
        return parseForStmt();
    }
    if (c.match(TokenType::KEYWORD_INSTRUMENT))
    {
        return parseClassDeclStmt();
    }

    if (c.match(TokenType::IDENTIFIER))
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
    if (c.match(TokenType::ASSIGNMENT_OP_EQUAL))
    {
        c.advance();
        init = exprParser.parseExpr();
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    auto stmt = std::make_unique<VarDeclStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        init != nullptr,
        std::move(init),
        Var(std::move(type), name)
    );

    if (!isField) c.getSymTable().addVar(stmt->getVar(), nameToken);
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
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::unique_ptr<Call>(dynamic_cast<Call*>(left.release())),
        opToken.value.value_or("="),
        std::move(right)
    );
}

std::unique_ptr<HearStmt> StmtParser::parseHearStmt()
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto var = exprParser.parseCallExpr();
    if (!var) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<HearStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(var)
    );
}

std::unique_ptr<PlayStmt> StmtParser::parsePlayStmt()
{
    const Token& t = c.current();
    bool isBar = c.match(TokenType::KEYWORD_PLAYBAR);
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto expr = exprParser.parseExpr();
    if (!expr) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<PlayStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(expr),
        isBar
    );
}

std::unique_ptr<BodyStmt> StmtParser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
{
    const Token& t = c.current();
    if (hasBrace)
    {
        if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;
    }

    c.getSymTable().enterScope(isBreakable, isContinueAble);
    for (const auto& arg : args)
    {
        c.getSymTable().addVar(arg.first, arg.second);
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    while (!c.isAtEnd() && (!hasBrace || !c.match(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET)))
    {
        if (c.match(TokenType::PUNCTUATION_REST)) {
            c.advance();
            if (c.match(TokenType::PUNCTUATION_SEMICOLON) || c.match(TokenType::PUNCTUATION_CLOSE_FILE)) c.advance();
            continue;
        }

        auto stmt = parseStmt(isGlobal, isBreakable, isContinueAble);
        if (stmt) bodyStmts.push_back(std::move(stmt));
        else c.synchronize();

        if (!hasBrace && isGlobal) break; // In global scope we parse one by one in parse()
    }

    if (hasBrace)
    {
        if (!c.expect(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;
    }

    auto scope = c.getSymTable().getCurrScope();
    c.getSymTable().exitScope();

    return std::make_unique<BodyStmt>(
        t,
        scope,
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        bodyStmts,
        isGlobal
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

    std::vector<std::pair<Var, const Token>> args;
    while (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        if (!type) return nullptr;

        Token argNameToken;
        if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), argNameToken)) return nullptr;

        args.emplace_back(Var(std::move(type), argNameToken.value.value_or("")), argNameToken);

        if (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;
        }
    }
    c.advance();

    std::unique_ptr<IType> retType = nullptr;
    if (c.match(TokenType::PUNCTUATION_RET_TYPE_ARROW))
    {
        c.advance();
        retType = typeParser.parseIType();
    }
    else
    {
        retType = std::make_unique<Type>("fermata");
    }

    // Handle copyright/credits if any?
    std::vector<std::unique_ptr<FuncCreditStmt>> credited;
    if (c.match(TokenType::PUNCTUATION_COPYRIGHT)) {
        c.advance();
        if (c.match(TokenType::PUNCTUATION_PARENTHESIS_OPEN)) {
            c.advance();
            while (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) {
                auto credit = parseFuncCreditStmt();
                if (credit) credited.push_back(std::move(credit));
                if (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
            }
            c.advance();
        }
    }

    auto funcStmt = std::make_unique<FuncDeclStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        name,
        std::move(retType),
        args,
        credited,
        isMethod
    );

    c.getSymTable().addFunc(funcStmt->getFunc(), nameToken);
    c.getSymTable().setCurrFunc(funcStmt.get());

    funcStmt->setBody(parseBodyStmt(args, false, false, false, true));

    c.getSymTable().setCurrFunc(nullptr);

    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt()
{
    const Token& t = c.current();
    c.advance();

    std::unique_ptr<Expr> expr = nullptr;
    if (!c.match(TokenType::PUNCTUATION_SEMICOLON) && !c.match(TokenType::PUNCTUATION_CLOSE_FILE))
    {
        if (c.match(TokenType::PUNCTUATION_BACKSLASH)) c.advance();
        expr = exprParser.parseExpr();
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<ReturnStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(expr)
    );
}

std::unique_ptr<FuncCreditStmt> StmtParser::parseFuncCreditStmt()
{
    const Token& t = c.current();
    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;

    return std::make_unique<FuncCreditStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        nameToken.value.value_or("")
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
    if (c.match(TokenType::KEYWORD_ELSE))
    {
        c.advance();
        if (c.match(TokenType::KEYWORD_IF)) elseBody = parseIfStmt();
        else elseBody = parseBodyStmt({}, false, false, false, true);
    }

    return std::make_unique<IfStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(cond),
        std::move(thenBody),
        std::move(elseBody)
    );
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt()
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    if (!type) return nullptr;

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;

    std::unique_ptr<Expr> size = nullptr;
    if (c.match(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE)) {
        c.advance();
        size = exprParser.parseExpr();
        c.expect(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE);
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    auto stmt = std::make_unique<ArrayDeclStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        Var(std::move(type), nameToken.value.value_or("")),
        std::move(size)
    );
    c.getSymTable().addVar(stmt->getVar(), nameToken);
    return stmt;
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
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(cond),
        std::move(body)
    );
}

std::unique_ptr<BreakStmt> StmtParser::parseBreakStmt()
{
    const Token& t = c.current();
    c.advance();
    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    return std::make_unique<BreakStmt>(t, c.getSymTable().getCurrScope(), c.getSymTable().getCurrFunc(), c.getSymTable().getCurrClass());
}

std::unique_ptr<CaseStmt> StmtParser::parseCaseStmt()
{
    const Token& t = c.current();
    c.advance();

    auto val = exprParser.parseExpr();
    if (!val) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_COLON, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;

    return std::make_unique<CaseStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(val),
        nullptr // body set by switch
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt()
{
    const Token& t = c.current();
    c.advance();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
    auto expr = exprParser.parseExpr();
    if (!expr) return nullptr;
    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.match(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.match(TokenType::KEYWORD_CASE))
        {
            auto cs = parseCaseStmt();
            if (cs) cases.push_back(std::move(cs));
        }
        else c.advance(); // ignore for now
    }
    c.advance();

    return std::make_unique<SwitchStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        std::move(expr),
        cases
    );
}

std::unique_ptr<ContinueStmt> StmtParser::parseContinueStmt()
{
    const Token& t = c.current();
    c.advance();
    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FILE, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;
    return std::make_unique<ContinueStmt>(t, c.getSymTable().getCurrScope(), c.getSymTable().getCurrFunc(), c.getSymTable().getCurrClass());
}

std::unique_ptr<ForStmt> StmtParser::parseForStmt()
{
    const Token& t = c.current();
    bool isUp = c.match(TokenType::KEYWORD_FMAJ);
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
    if (c.match(TokenType::PUNCTUATION_COLON)) {
        c.advance();
        step = exprParser.parseExpr();
    }
    
    c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE);

    auto body = parseBodyStmt({}, false, true, true, true);

    return std::make_unique<ForStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        Var(std::move(type), varToken.value.value_or("")),
        std::move(start),
        std::move(stop),
        std::move(step),
        std::move(body),
        isUp
    );
}

std::unique_ptr<ClassDeclStmt> StmtParser::parseClassDeclStmt()
{
    const Token& t = c.current();
    c.advance();

    Token nameToken;
    c.expect(TokenType::IDENTIFIER, nullptr, nameToken);
    
    std::string parentName = "";
    if (c.match(TokenType::PUNCTUATION_COLON)) {
        c.advance();
        Token pToken;
        c.expect(TokenType::IDENTIFIER, nullptr, pToken);
        parentName = pToken.value.value_or("");
    }

    auto stmt = std::make_unique<ClassDeclStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        nameToken.value.value_or(""),
        parentName
    );
    
    c.getSymTable().addClass(stmt->getCurrClass(), nameToken);
    c.getSymTable().setCurrClass(stmt.get());
    
    c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET);
    
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;
    
    while (!c.match(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET)) {
        if (c.match(TokenType::KEYWORD_PLAYERSCORE) || c.match(TokenType::KEYWORD_CONDUCTORSCORE) || c.match(TokenType::KEYWORD_SECTIONSCORE)) {
             // handle access
             c.advance();
        }
        
        if (c.match(TokenType::KEYWORD_BASS)) {
            auto ctor = parseCtor();
            if (ctor) ctors.emplace_back(AccessType::PRIVATE, std::move(ctor));
        } else if (c.match(TokenType::KEYWORD_SONG)) {
            auto method = parseFuncDeclStmt(true);
            if (method) methods.emplace_back(AccessType::PRIVATE, std::move(method));
        } else if (c.isType()) {
            auto field = parseVarDecStmt(true);
            if (field) fields.emplace_back(AccessType::PRIVATE, std::move(field));
        } else {
            c.advance();
        }
    }
    c.advance();
    
    stmt->setFields(fields);
    stmt->setMethods(methods);
    stmt->setCtors(ctors);
    
    c.getSymTable().setCurrClass(nullptr);
    return stmt;
}

std::unique_ptr<ConstractorDeclStmt> StmtParser::parseCtor()
{
    const Token& t = c.current();
    c.advance();
    
    c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN);
    // parse args...
    std::vector<std::pair<Var, const Token>> args;
    while (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) {
        auto type = typeParser.parseIType();
        Token n;
        c.expect(TokenType::IDENTIFIER, nullptr, n);
        args.emplace_back(Var(std::move(type), n.value.value_or("")), n);
        if (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
    }
    c.advance();
    
    auto stmt = std::make_unique<ConstractorDeclStmt>(
        t,
        c.getSymTable().getCurrScope(),
        nullptr,
        c.getSymTable().getCurrClass(),
        args
    );
    
    stmt->setBody(parseBodyStmt(args, false, false, false, true));
    return stmt;
}

std::unique_ptr<ConstractorCallStmt> StmtParser::parseConstractorCallStmt()
{
    const Token& t = c.current();
    c.advance();
    // ...
    return nullptr;
}

std::unique_ptr<ObjCreationStmt> StmtParser::parseObjCreationStmt()
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    Token name;
    c.expect(TokenType::IDENTIFIER, nullptr, name);
    
    std::vector<std::unique_ptr<Expr>> args;
    if (c.match(TokenType::PUNCTUATION_PARENTHESIS_OPEN)) {
        c.advance();
        while (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) {
            args.push_back(exprParser.parseExpr());
            if (!c.match(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
        }
        c.advance();
    }
    
    c.expect(TokenType::PUNCTUATION_SEMICOLON);
    
    return std::make_unique<ObjCreationStmt>(
        t,
        c.getSymTable().getCurrScope(),
        c.getSymTable().getCurrFunc(),
        c.getSymTable().getCurrClass(),
        Var(std::move(type), name.value.value_or("")),
        args
    );
}

bool StmtParser::parseFields(std::vector<Field>& fields) { return true; }
bool StmtParser::parseMethods(std::vector<Method>& methods) { return true; }
bool StmtParser::parseCtors(std::vector<Ctor>& ctors) { return true; }
