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

#include "errorHandling/semanticErrors/StmtNotBreakable.h"
#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

#include "errorHandling/entryPointErrors/MainOverride.h"

#include "errorHandling/classErrors/NoCtor.h"
#include "errorHandling/classErrors/InvalidAccessKeyword.h"
#include "errorHandling/classErrors/InvalidCtorName.h"
#include "errorHandling/classErrors/MissingClassPipe.h"
#include "errorHandling/classErrors/NoOverrideError.h"
#include "errorHandling/classErrors/StaticCtor.h"
#include "errorHandling/how/HowDareYou.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/lexicalErrors/UnrecognizedToken.h"
#include "errorHandling/semanticErrors/StaticFuncCantVirtual.h"
#include "errorHandling/syntaxErrors/ExpectedAnExpression.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/NoLineOpener.h"
#include "errorHandling/syntaxErrors/NoNewLine.h"
#include "errorHandling/syntaxErrors/NoPlacementOperator.h"
#include "errorHandling/syntaxErrors/NoRest.h"

StmtParser::StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser)
    : c(c), typeParser(typeParser), exprParser(exprParser)
{
}

SemiColonEatType StmtParser::expectSemiColon(const bool isInFunc) const // TODO when match and in func expect new line and call eat func new line ALWAYSS
{
    if (!isInFunc)
    {
        if (!c.matchConsume(TokenType::PUNCTUATION_SEMICOLON))
        {
            c.addError(std::make_unique<MissingSemicolon>(c.current()));
            return SemiColonEatType::IN_SCOPE;
        }
        return SemiColonEatType::IN_SCOPE;
    }

    if (c.matchConsume(TokenType::PUNCTUATION_CLOSE_FUNC))
    {
        return SemiColonEatType::OUT_SCOPE;
    }
    if (c.matchConsume(TokenType::PUNCTUATION_SEMICOLON))
    {
        return SemiColonEatType::IN_SCOPE;
    }

    c.addError(std::make_unique<MissingSemicolon>(c.current()));
    return SemiColonEatType::IN_SCOPE;
}

void StmtParser::eatFuncNewLine() const
{
    if (!c.matchConsume(TokenType::PUNCTUATION_NEW_LINE))
    {
        return;
    }
    c.expect(TokenType::PUNCTUATION_OPEN_LINE, std::make_unique<NoLineOpener>(c.current()));

    if (c.matchConsume(TokenType::PUNCTUATION_SEMICOLON) || c.matchConsume(TokenType::PUNCTUATION_CLOSE_FUNC))
    {
        c.addError(std::make_unique<NoRest>(c.current()));
    }
    eatRest();
}

void StmtParser::eatRest() const
{
    if (!c.matchConsume(TokenType::PUNCTUATION_REST))
    {
        return;
    }
    if (expectSemiColon(true) == SemiColonEatType::OUT_SCOPE) return;
    eatFuncNewLine();
}

void StmtParser::parse()
{
    while (!c.isEmpty())
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
    if (c.matchConsume(TokenType::KEYWORD_C_CLEF))
    {
        throw HowDareYou(c.current());
    }

    if (c.matchConsume(TokenType::TYPE_RIFF))
    {
        return parseArrayDeclStmt();
    }

    if (c.isType())
    {
        return parseVarDecStmt();
    }

    if (c.matchNonConsume(TokenType::IDENTIFIER))
    {
        if (c.matchConsume(TokenType::IDENTIFIER) && c.isUnaryOpStmtAhead())
        {
            auto expr = exprParser.parseUnaryOpExpr(true);
            return std::unique_ptr<Stmt>(dynamic_cast<Stmt*>(expr.release()));
        }
        if (c.matchConsume(TokenType::IDENTIFIER) && c.isAssignmentStmtAhead())
        {
            return parseAssignmentStmt();
        }

        if (c.matchConsume(TokenType::IDENTIFIER))
        {
            // Might be a function call as a statement
            if (c.peek().type == TokenType::PUNCTUATION_PARENTHESIS_OPEN)
            {
                auto expr = exprParser.parseFuncCallExpr(true);
                return std::unique_ptr<Stmt>(dynamic_cast<Stmt*>(expr.release()));
            }
        }
    }

    if (c.matchConsume(TokenType::KEYWORD_HEAR))
    {
        return parseHearStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_PLAY) || c.matchConsume(TokenType::KEYWORD_PLAYBAR))
    {
        return parsePlayStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_SONG))
    {
        return parseFuncDeclStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_RETURN))
    {
        return parseReturnStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_IF))
    {
        return parseIfStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_WHILE))
    {
        return parseWhileStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_SWITCH))
    {
        return parseSwitchStmt();
    }
    if (c.matchNonConsume(TokenType::KEYWORD_FMAJ) || c.matchConsume(TokenType::KEYWORD_FMIN))
    {
        return parseForStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_INSTRUMENT))
    {
        return parseClassDeclStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_CTOR_CALL))
    {
        return parseObjCreationStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_PAUSE))
    {
        if (!isBreakable)
        {
            c.addError(std::make_unique<StmtNotBreakable>(c.current()));
        }
        return parseBreakStmt();
    }
    if (c.matchConsume(TokenType::KEYWORD_RESUME))
    {
        if (!isContinueAble)
        {
            c.addError(std::make_unique<StmtNotContinueAble>(c.current()));
        }
        return parseContinueStmt();
    }

    c.addError(std::make_unique<UnrecognizedToken>(c.current()));
    return nullptr;
}

std::unique_ptr<VarDeclStmt> StmtParser::parseVarDecStmt() const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(t));
        return nullptr;
    }

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;
    const std::string name = nameToken.value.value();

    std::unique_ptr<Expr> init = nullptr;
    if (c.matchConsume(TokenType::ASSIGNMENT_OP_EQUAL))
    {
        init = exprParser.parseExpr();

        if (init == nullptr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.current()));
            return nullptr;
        }
    }

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
    auto left = exprParser.parseVarCallExpr(true);
    if (!left) return nullptr;

    if (!c.isAssignmentOp())
    {
        c.addError(std::make_unique<NoPlacementOperator>(c.current()));
        return nullptr;
    }
    const Token opToken = c.advance();

    auto right = exprParser.parseExpr();
    if (!right) return nullptr;

    return std::make_unique<AssignmentStmt>(
        t,
        c.getFuncDecl(),
        std::move(left),
        opToken.value.value(),
        std::move(right),
        c.getClassDecl()
    );
}

std::unique_ptr<HearStmt> StmtParser::parseHearStmt() const
{
    const Token& t = c.current();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto var = exprParser.parseCallExpr();
    if (!var)
    {
        c.addError(std::make_unique<ExpectedAnExpression>(t));
        return nullptr;
    }

    std::vector<std::unique_ptr<Call>> calls;
    calls.push_back(std::move(var));

    while (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        var = exprParser.parseCallExpr();
        calls.push_back(std::move(var));

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.current()));
        }
    }

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
    bool isBar = c.advance().type == TokenType::KEYWORD_PLAYBAR;

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<Expr>> exprs;

    while (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto expr = exprParser.parseExpr(false, false);
        if (!expr) {
            c.addError(std::make_unique<ExpectedAnExpression>(t));
            return nullptr;
        }

        exprs.push_back(std::move(expr));

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.current()));
        }
    }

    return std::make_unique<PlayStmt>(
        t,
        c.getFuncDecl(),
        std::move(exprs),
        isBar,
        c.getClassDecl()
    );
}

std::unique_ptr<BodyStmt> StmtParser::parseBodyStmt(const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
{
    const Token& t = c.current();
    if (hasBrace)
    {
        if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    while (hasBrace && !c.matchConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        eatFuncNewLine();

        if (auto stmt = parseStmt(isGlobal, isBreakable, isContinueAble)) bodyStmts.push_back(std::move(stmt));

        if (expectSemiColon(true) == SemiColonEatType::OUT_SCOPE)
        {
            break;
        }
    }

    if (hasBrace) c.expect(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()));

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

    std::vector<std::unique_ptr<FuncCreditStmt>> credited;

    if (c.matchConsume(TokenType::PUNCTUATION_COPYRIGHT))
    {
        if (c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
        {
            while (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
            {
                if (auto credit = parseFuncCreditStmt()) credited.push_back(std::move(credit));
                if (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()));
            }
        }
    }

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;
    std::string name = nameToken.value.value();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    std::vector<Var> args;
    std::vector<std::pair<Var, const Token>> argsWithTokens;
    while (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        if (!type) return nullptr;

        Token argNameToken;
        if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), argNameToken)) return nullptr;

        args.emplace_back(std::move(type), argNameToken.value.value());
        argsWithTokens.emplace_back(args.back(), argNameToken);

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(TokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.current()))) return nullptr;
        }
    }

    std::unique_ptr<IType> retType = nullptr;
    if (c.matchConsume(TokenType::PUNCTUATION_RET_TYPE_ARROW))
    {
        retType = typeParser.parseIType();
    }
    else
    {
        retType = std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA);
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

    funcStmt->setBody(parseBodyStmt(false, false, false, true));

    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt() const
{
    c.expect(TokenType::PUNCTUATION_BACKSLASH, std::make_unique<UnexpectedToken>(c.current()));
    auto expr = exprParser.parseExpr();

    return std::make_unique<ReturnStmt>(
        c.current(),
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

StmtWithBody StmtParser::getIfStmtWithBody()
{
    const Token& t = c.current();

    std::unique_ptr<Expr> condition = nullptr;

    if (c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_OPEN)) // if or else if
    {
        condition = exprParser.parseExpr();
        if (!condition)
        {
            c.addError(std::make_unique<InvalidExpression>(t));
            return {nullptr, nullptr};
        }

        if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return {nullptr, nullptr};
    }
    auto body = parseBodyStmt(false, false, false, true);

    return {condition ? std::move(condition) : nullptr, std::move(body)};
}

std::unique_ptr<IfStmt> StmtParser::parseIfStmt()
{
    const Token& t = c.current();

    auto currStmt = getIfStmtWithBody();

    std::vector<StmtWithBody> elses;
    while (c.matchConsume(TokenType::KEYWORD_ELSE))
    {
        if (c.matchConsume(TokenType::PUNCTUATION_BACKSLASH)) // else if
        {
            c.expect(TokenType::KEYWORD_IF, std::make_unique<ExpectedKeywordNotFound>(c.current(), "D"));
            elses.push_back(getIfStmtWithBody());
        }

        else // else
        {
            elses.push_back(getIfStmtWithBody());
            break;
        }

    }

    return std::make_unique<IfStmt>(
        t,
        c.getFuncDecl(),
        std::move(currStmt),
        elses,
        c.getClassDecl()
    );
}

std::unique_ptr<WhileStmt> StmtParser::parseWhileStmt()
{
    const Token& t = c.current();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;
    auto condition = exprParser.parseExpr();
    if (!condition) return nullptr;
    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    auto body = parseBodyStmt(false, true, true, true);

    auto stmt = StmtWithBody(std::move(condition), std::move(body));

    return std::make_unique<WhileStmt>(
        t,
        c.getFuncDecl(),
        stmt,
        c.getClassDecl()
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt()
{
    const Token& t = c.current();

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (const auto expr = exprParser.parseExpr(); !expr)
    {
        c.addError(std::make_unique<InvalidExpression>(t));
        return nullptr;
    }

    if (!c.expect(TokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.current()))) return nullptr;

    if (!c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.current()))) return nullptr;

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.matchConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.matchConsume(TokenType::KEYWORD_CASE))
        {
            if (auto cs = parseCaseStmt()) cases.push_back(std::move(cs));
        }
        else c.advance(); // ignore for now
    }

    return std::make_unique<SwitchStmt>(
        t,
        c.getFuncDecl(),
        Var(nullptr, ""), // dummy var
        cases,
        c.getClassDecl()
    );
}

std::unique_ptr<CaseStmt> StmtParser::parseCaseStmt()
{
    const Token& t = c.current();

    bool isDefault = false;

    std::unique_ptr<Expr> caseing;

    if (c.matchConsume(TokenType::PUNCTUATION_BACKSLASH))
    {
        caseing = exprParser.parseExpr();

        if (!caseing)
            c.addError(std::make_unique<InvalidExpression>(t)); return nullptr;
    }

    else
    {
        isDefault = true;
    }

    constexpr std::vector<std::pair<Var, const Token>> emptyArgs = {};
    auto body = parseBodyStmt(false, true, false, false);

    StmtWithBody ret(std::move(caseing), std::move(body));

    return std::make_unique<CaseStmt>(
        t,
        c.getFuncDecl(),
        std::move(ret),
        isDefault,
        c.getClassDecl()
    );
}

std::unique_ptr<BreakStmt> StmtParser::parseBreakStmt() const
{
    expectSemiColon(true);
    return std::make_unique<BreakStmt>(c.current(), c.getFuncDecl(), c.getClassDecl());
}

std::unique_ptr<ContinueStmt> StmtParser::parseContinueStmt() const
{
    expectSemiColon(true);
    return std::make_unique<ContinueStmt>(c.current(), c.getFuncDecl(), c.getClassDecl());
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt() const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();

    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(t));
        return nullptr;
    }

    Token nameToken;
    if (!c.expect(TokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.current()), nameToken)) return nullptr;

    std::vector<std::unique_ptr<Expr>> sizes;
    while (c.matchConsume(TokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        sizes.push_back(exprParser.parseExpr());
        c.expect(TokenType::PUNCTUATION_CLOSE_SQUARE_BRACE);
    }

    if (!c.expect(TokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.current())) &&
        !c.expect(TokenType::PUNCTUATION_CLOSE_FUNC, std::make_unique<MissingSemicolon>(c.current()))) return nullptr;

    return std::make_unique<ArrayDeclStmt>(
        t,
        c.getFuncDecl(),
        false, // hasStartingValue
        nullptr, // startingValue
        Var(std::move(type), nameToken.value.value_or("")),
        std::move(sizes),
        c.getClassDecl()
    );
} // TODO check


std::unique_ptr<ForStmt> StmtParser::parseForStmt()
{
     const Token& t = c.current();
     bool isIncreasing;

    if (c.matchConsume(TokenType::KEYWORD_FMAJ))
    {
        isIncreasing = true;
    }
    else if (c.matchConsume(TokenType::KEYWORD_FMIN))
    {
        isIncreasing = false;
    }
    else
    {
        c.addError(std::make_unique<HowDidYouGetHere>(c.current()));
        return nullptr;
    }

     std::unique_ptr<Expr> startExpr = nullptr;
     std::unique_ptr<Expr> stopExpr = nullptr;
     std::unique_ptr<Expr> stepExpr = nullptr;
     std::unique_ptr<BodyStmt> body = nullptr;
     std::string varName = "i";
     Token varToken;

    startExpr = exprParser.parseExpr();
    if (!startExpr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.current()));
        return nullptr;
    }

    if ((isIncreasing && c.matchConsume(TokenType::UNARY_OP_SHARP)) || !isIncreasing && c.matchConsume(TokenType::UNARY_OP_FLAT))
    {
        stepExpr = exprParser.parseExpr();
        if (!stepExpr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.current()));
            return nullptr;
        }
    }

    if (c.matchConsume(TokenType::UNARY_OP_SHARP))
    {
        stopExpr = exprParser.parseExpr();
        if (!stopExpr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.current()));
            return nullptr;
        }
    }

    if (c.matchConsume(TokenType::PUNCTUATION_BACKSLASH))
        c.expect(TokenType::IDENTIFIER, nullptr, varToken);

    body = parseBodyStmt(false, true, true, true);

    return std::make_unique<ForStmt>(
        t,
        c.getFuncDecl(),
        std::move(body),
        isIncreasing,
        std::move(startExpr),
        std::move(stepExpr),
        std::move(stopExpr),
        varToken.value.value_or("i"),
        c.getClassDecl()
    );
}

std::unique_ptr<ClassDeclStmt> StmtParser::parseClassDeclStmt()
{
    const Token& t = c.current();

    Token nameToken;
    c.expect(TokenType::IDENTIFIER, nullptr, nameToken);
    std::string className = nameToken.value.value();

    std::string parentName = "";
    AccessType inheretingPublicity = NONE;

    if (c.matchConsume(TokenType::PUNCTUATION_COLON))
    {
        if (c.matchConsume(TokenType::KEYWORD_TUTTI))
        {
            inheretingPublicity = PUBLIC;
        }
        else if (c.matchConsume(TokenType::KEYWORD_SORDINO))
        {
            inheretingPublicity = PRIVATE;
        }
        else if (c.matchConsume(TokenType::KEYWORD_SECTION))
        {
            inheretingPublicity = PROTECTED;
        }
        else
        {
            c.addError(std::make_unique<InvalidAccessKeyword>(c.current()));
            return nullptr;
        }

        Token pToken;
        c.expect(TokenType::IDENTIFIER, nullptr, pToken);
        parentName = pToken.value.value_or("");
    }

    c.expect(TokenType::PUNCTUATION_OPEN_CURLY_BRACKET);

    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    while (!c.matchConsume(TokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        bool isStatic = false;
        auto virtualType = VirtualType::NONE;

        AccessType access = PUBLIC;
        if (c.matchConsume(TokenType::KEYWORD_PLAYERSCORE)) access = PUBLIC;
        else if (c.matchConsume(TokenType::KEYWORD_CONDUCTORSCORE)) access = PRIVATE;
        else if (c.matchConsume(TokenType::KEYWORD_SECTIONSCORE)) access = PROTECTED;

        if (c.matchConsume(TokenType::KEYWORD_UNISON))
        {
            isStatic = true;
        }
        if (c.matchConsume(TokenType::KEYWORD_VARIATION))
        {
            if (isStatic)
            {
                c.addError(std::make_unique<StaticFuncCantVirtual>(c.current()));
                continue;
            }
            virtualType = VirtualType::OVERRIDE;
        }
        if (c.matchConsume(TokenType::KEYWORD_REST))
        {
            if (isStatic || virtualType != VirtualType::NONE)
            {
                c.addError(std::make_unique<UnexpectedToken>(c.current()));
                continue;
            }
            virtualType = VirtualType::PURE;
        }
        if (c.matchConsume(TokenType::KEYWORD_MOTIF))
        {
            if (isStatic || virtualType != VirtualType::NONE)
            {
                c.addError(std::make_unique<UnexpectedToken>(c.current()));
                continue;
            }
            virtualType = VirtualType::VIRTUAL;
        }

        if (c.matchConsume(TokenType::KEYWORD_CTOR_CALL))
        {
            if (auto ctor = parseCtor(className)) ctors.emplace_back(access, std::move(ctor));
            if (isStatic)
            {
                c.addError(std::make_unique<StaticCtor>(c.current()));
                continue;
            }
            if (virtualType != VirtualType::NONE)
            {
                c.addError()
            }
        }

        else if (c.matchConsume(TokenType::KEYWORD_SONG))
        {
            if (auto method = parseFuncDeclStmt(true)) methods.emplace_back(access, std::move(method));
        }

        else if (c.isType())
        {
            if (auto field = parseVarDecStmt()) fields.emplace_back(access, std::move(field));
        }

        else
        {
            c.addError(std::make_unique<UnexpectedToken>(c.current()));
        }

    }

    return std::make_unique<ClassDeclStmt>(
        t,
        c.getFuncDecl(),
        className,
        fields,
        methods,
        ctors,
        !parentName.empty(),
        inheretingPublicity,
        parentName,
        c.getClassDecl()
    );
}

std::unique_ptr<ConstractorDeclStmt> StmtParser::parseCtor(const std::string& className)
{
    const Token& t = c.advance();

    if (t.value.value() != className)
    {
        c.addError(std::make_unique<InvalidCtorName>(t, className));
    }

    std::vector<Var> args;
    std::vector<std::pair<Var, const Token>> argsWithTokens;

    c.expect(TokenType::PUNCTUATION_PARENTHESIS_OPEN);
    while (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        Token n;

        auto type = typeParser.parseIType();
        c.expect(TokenType::IDENTIFIER, nullptr, n);

        args.emplace_back(std::move(type), n.value.value());
        argsWithTokens.emplace_back(args.back(), n);

        if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
    }

    auto stmt = std::make_unique<ConstractorDeclStmt>(
        t,
        className,
        args,
        c.getClassDecl()
    );

    stmt->setBody(parseBodyStmt(false, false, false, true));
    return stmt;
}

std::unique_ptr<ObjCreationStmt> StmtParser::parseObjCreationStmt() const
{
    const Token& t = c.current();
    auto type = typeParser.parseIType();
    Token name;
    c.expect(TokenType::IDENTIFIER, nullptr, name);

    std::vector<std::unique_ptr<Expr>> args;
    if (c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        while (!c.matchConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            args.push_back(exprParser.parseExpr());
            if (!c.matchNonConsume(TokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(TokenType::PUNCTUATION_COMMA);
        }
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