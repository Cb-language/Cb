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
#include "errorHandling/syntaxErrors/ExpectedAnExpression.h"
#include "errorHandling/syntaxErrors/IncludeNotInTop.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/NoLineOpener.h"
#include "errorHandling/syntaxErrors/NoPlacementOperator.h"
#include "errorHandling/syntaxErrors/NoRest.h"
#include "errorHandling/semanticErrors/StmtNotBreakable.h"
#include "errorHandling/semanticErrors/StmtNotContinueAble.h"
#include "errorHandling/semanticErrors/StaticFuncCantVirtual.h"

#include "errorHandling/classErrors/InvalidAccessKeyword.h"
#include "errorHandling/classErrors/InvalidCtorName.h"
#include "errorHandling/classErrors/StaticCtor.h"
#include "errorHandling/classErrors/VirtualCtor.h"

#include "errorHandling/lexicalErrors/UnrecognizedToken.h"

#include "errorHandling/how/HowDareYou.h"
#include "errorHandling/how/HowDidYouGetHere.h"

StmtParser::StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser)
    : c(c), typeParser(typeParser), exprParser(exprParser)
{
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
    if (c.matchConsume(CbTokenType::KEYWORD_C_CLEF))
    {
        throw HowDareYou(c.copyCurrent());
    }
    if (c.matchNonConsume(CbTokenType::TYPE_RIFF))
    {
        return parseArrayDeclStmt();
    }
    if (c.isType())
    {
        return parseVarDecStmt();
    }

    if (c.matchNonConsume(CbTokenType::IDENTIFIER))
    {
        return exprParser.parseExpr();
    }

    if (c.matchConsume(CbTokenType::KEYWORD_HEAR))
    {
        return parseHearStmt();
    }
    if (c.matchNonConsume(CbTokenType::KEYWORD_PLAY) || c.matchNonConsume(CbTokenType::KEYWORD_PLAYBAR))
    {
        return parsePlayStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_SONG))
    {
        return parseFuncDeclStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_RETURN))
    {
        return parseReturnStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_IF))
    {
        return parseIfStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_WHILE))
    {
        return parseWhileStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_SWITCH))
    {
        return parseSwitchStmt();
    }
    if (c.matchNonConsume(CbTokenType::KEYWORD_FMAJ) || c.matchNonConsume(CbTokenType::KEYWORD_FMIN))
    {
        return parseForStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_INSTRUMENT))
    {
        return parseClassDeclStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_CTOR_CALL))
    {
        return parseObjCreationStmt();
    }
    if (c.matchConsume(CbTokenType::KEYWORD_PAUSE))
    {
        if (!isBreakable)
        {
            c.addError(std::make_unique<StmtNotBreakable>(c.copyCurrent()));
        }
        return std::make_unique<BreakStmt>(c.copyCurrent(), c.getFuncDecl(), c.getClassDecl());
    }
    if (c.matchConsume(CbTokenType::KEYWORD_RESUME))
    {
        if (!isContinueAble)
        {
            c.addError(std::make_unique<StmtNotContinueAble>(c.copyCurrent()));
        }
        return std::make_unique<BreakStmt>(c.copyCurrent(), c.getFuncDecl(), c.getClassDecl());
    }
    if (c.matchConsume(CbTokenType::KEYWORD_FEAT))
    {
        c.addError(std::make_unique<IncludeNotInTop>(c.copyCurrent()));
    }

    c.addError(std::make_unique<UnrecognizedToken>(c.copyCurrent()));
    c.advance();
    return nullptr;
}

std::unique_ptr<VarDeclStmt> StmtParser::parseVarDecStmt() const
{
    Token t = c.copyCurrent();
    auto type = typeParser.parseIType();
    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(t));
        return nullptr;
    }

    const FQN name = c.parseFQN();

    std::unique_ptr<Expr> init = nullptr;
    if (c.matchConsume(CbTokenType::BINARY_OP_EQUAL))
    {
        init = exprParser.parseExpr();

        if (init == nullptr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
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

std::unique_ptr<AssignmentStmt> StmtParser::parseAssignmentStmt(std::unique_ptr<Call> left) const
{
    if (!c.isBinaryOp())
    {
        c.addError(std::make_unique<NoPlacementOperator>(c.copyCurrent()));
        return nullptr;
    }
    Token opToken;
    c.matchConsume(c.current().type, opToken);

    auto right = exprParser.parseExpr();
    if (!right) return nullptr;

    return std::make_unique<AssignmentStmt>(
        opToken,
        c.getFuncDecl(),
        std::move(left),
        opToken.value.value(),
        std::move(right),
        c.getClassDecl()
    );
}

std::unique_ptr<HearStmt> StmtParser::parseHearStmt() const
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;

    auto var = exprParser.parseCallExpr();
    if (!var)
    {
        c.addError(std::make_unique<ExpectedAnExpression>(c.copyCurrent()));
        return nullptr;
    }

    std::vector<std::unique_ptr<Call>> calls;
    calls.push_back(std::move(var));

    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        var = exprParser.parseCallExpr();
        calls.push_back(std::move(var));

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.copyCurrent()));
        }
    }

    return std::make_unique<HearStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        calls,
        c.getClassDecl()
    );
}

std::unique_ptr<PlayStmt> StmtParser::parsePlayStmt() const
{
    Token t;
    bool isBar = false;
    if (c.matchConsume(CbTokenType::KEYWORD_PLAYBAR, t)) isBar = true;
    else c.matchConsume(CbTokenType::KEYWORD_PLAY, t);

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;

    std::vector<std::unique_ptr<Expr>> exprs;

    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto expr = exprParser.parseExpr();
        if (!expr) {
            c.addError(std::make_unique<ExpectedAnExpression>(t));
            return nullptr;
        }

        exprs.push_back(std::move(expr));

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.copyCurrent()));
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
    Token t = c.copyCurrent();
    if (hasBrace)
    {
        if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.copyCurrent()))) return nullptr;
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    while (!hasBrace || !c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET)) // if  it doesn't have brace will automatically exit at the first endScope symbol
    {
        if (auto stmt = parseStmt(isGlobal, isBreakable, isContinueAble)) bodyStmts.push_back(std::move(stmt));

        c.expectSemiColon();
        if (!c.getIsInFunc())
        {
            break;
        }
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

    std::vector<std::unique_ptr<FuncCreditStmt>> credited;

    if (c.matchConsume(CbTokenType::PUNCTUATION_COPYRIGHT))
    {
        if (c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;
        {
            while (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
            {
                if (auto credit = parseFuncCreditStmt()) credited.push_back(std::move(credit));
                if (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.copyCurrent()));
            }
        }
    }

    const FQN name = c.parseFQN();

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;

    std::vector<Var> args;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        if (!type) return nullptr;

        const FQN argName = c.parseFQN();
        args.emplace_back(std::move(type), argName);

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.copyCurrent()))) return nullptr;
        }
    }

    std::unique_ptr<IType> retType = nullptr;
    if (c.matchConsume(CbTokenType::PUNCTUATION_RET_TYPE_ARROW))
    {
        c.setIsInFunc(true);
        retType = typeParser.parseIType();
    }
    else
    {
        c.setIsInFunc(true);
        retType = std::make_unique<PrimitiveType>(Primitive::TYPE_FERMATA);
    }

    auto funcStmt = std::make_unique<FuncDeclStmt>(
        c.copyCurrent(),
        name,
        std::move(retType),
        args,
        credited,
        isMethod,
        VirtualType::NONE,
        false,
        c.getClassDecl()
    );

    const auto temp = c.getFuncDecl();

    c.setFuncDecl(funcStmt.get());

    funcStmt->setBody(parseBodyStmt(false, false, false, false));

    c.setFuncDecl(temp);
    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt() const
{
    c.expect(CbTokenType::PUNCTUATION_BACKSLASH, std::make_unique<UnexpectedToken>(c.copyCurrent()));
    auto expr = exprParser.parseExpr();

    return std::make_unique<ReturnStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        expr,
        nullptr, 
        c.getClassDecl()
    );
}

std::unique_ptr<FuncCreditStmt> StmtParser::parseFuncCreditStmt() const
{
    Token t = c.copyCurrent();
    const FQN name = c.parseFQN();

    return std::make_unique<FuncCreditStmt>(
        t,
        c.getFuncDecl(),
        FuncCredit(name, t),
        c.getClassDecl()
    );
}

StmtWithBody StmtParser::getIfStmtWithBody()
{
    Token t = c.copyCurrent();

    std::unique_ptr<Expr> condition = nullptr;

    if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN)) 
    {
        condition = exprParser.parseExpr();
        if (!condition)
        {
            c.addError(std::make_unique<InvalidExpression>(t));
            return {nullptr, nullptr};
        }

        if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return {nullptr, nullptr};
    }
    auto body = parseBodyStmt(false, false, false, true);

    return {condition ? std::move(condition) : nullptr, std::move(body)};
}

std::unique_ptr<IfStmt> StmtParser::parseIfStmt()
{
    auto currStmt = getIfStmtWithBody();

    std::vector<StmtWithBody> elses;
    while (c.matchConsume(CbTokenType::KEYWORD_ELSE))
    {
        if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH)) 
        {
            c.expect(CbTokenType::KEYWORD_IF, std::make_unique<ExpectedKeywordNotFound>(c.copyCurrent(), "D"));
            elses.push_back(getIfStmtWithBody());
        }
        else 
        {
            elses.push_back(getIfStmtWithBody());
            break;
        }
    }

    return std::make_unique<IfStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        std::move(currStmt),
        elses,
        c.getClassDecl()
    );
}

std::unique_ptr<WhileStmt> StmtParser::parseWhileStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;
    auto condition = exprParser.parseExpr();
    if (!condition) return nullptr;
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;

    auto body = parseBodyStmt(false, true, true, true);
    auto stmt = StmtWithBody(std::move(condition), std::move(body));

    return std::make_unique<WhileStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        stmt,
        c.getClassDecl()
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;

    if (const auto expr = exprParser.parseExpr(); !expr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
        return nullptr;
    }

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent()))) return nullptr;
    if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.copyCurrent()))) return nullptr;

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.matchConsume(CbTokenType::KEYWORD_CASE))
        {
            if (auto cs = parseCaseStmt()) cases.push_back(std::move(cs));
        }
        else c.advance(); 
    }

    constexpr FQN empty;

    return std::make_unique<SwitchStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        Var(nullptr, empty), 
        cases,
        c.getClassDecl()
    );
}

std::unique_ptr<CaseStmt> StmtParser::parseCaseStmt()
{
    bool isDefault = false;
    std::unique_ptr<Expr> caseing;

    if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
    {
        caseing = exprParser.parseExpr();
        if (!caseing)
        {
            c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
            return nullptr;
        }
    }
    else
    {
        isDefault = true;
    }

    auto body = parseBodyStmt(false, true, false, false);
    StmtWithBody ret(std::move(caseing), std::move(body));

    return std::make_unique<CaseStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        std::move(ret),
        isDefault,
        c.getClassDecl()
    );
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt() const
{
    Token t = c.copyCurrent();
    c.matchConsume(CbTokenType::TYPE_RIFF);
    auto type = typeParser.parseIType();

    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(t));
        return nullptr;
    }

    const FQN name = c.parseFQN();

    std::vector<std::unique_ptr<Expr>> sizes;
    while (c.matchConsume(CbTokenType::PUNCTUATION_OPEN_SQUARE_BRACE))
    {
        sizes.push_back(exprParser.parseExpr());
        c.expect(CbTokenType::PUNCTUATION_CLOSE_SQUARE_BRACE);
    }

    if (!c.expect(CbTokenType::PUNCTUATION_SEMICOLON, std::make_unique<MissingSemicolon>(c.copyCurrent())) &&
        !c.expect(CbTokenType::PUNCTUATION_CLOSE_FUNC, std::make_unique<MissingSemicolon>(c.copyCurrent()))) return nullptr;

    return std::make_unique<ArrayDeclStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        false, 
        nullptr, 
        Var(std::move(type), name),
        std::move(sizes),
        c.getClassDecl()
    );
}

std::unique_ptr<ForStmt> StmtParser::parseForStmt()
{
    Token t = c.copyCurrent();
    bool isIncreasing;

    if (c.matchConsume(CbTokenType::KEYWORD_FMAJ)) isIncreasing = true;
    else if (c.matchConsume(CbTokenType::KEYWORD_FMIN)) isIncreasing = false;
    else return nullptr;

    std::unique_ptr<Expr> startExpr = exprParser.parseExpr();
    if (!startExpr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
        return nullptr;
    }

    std::unique_ptr<Expr> stepExpr = nullptr;
    if ((isIncreasing && c.matchConsume(CbTokenType::UNARY_OP_SHARP)) || (!isIncreasing && c.matchConsume(CbTokenType::UNARY_OP_FLAT)))
    {
        stepExpr = exprParser.parseExpr();
    }

    std::unique_ptr<Expr> stopExpr = nullptr;
    if (c.matchConsume(CbTokenType::UNARY_OP_SHARP))
    {
        stopExpr = exprParser.parseExpr();
    }

    Token varToken;
    if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
        c.expect(CbTokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.copyCurrent()), varToken);

    auto body = parseBodyStmt(false, true, true, true);

    return std::make_unique<ForStmt>(
        c.copyCurrent(),
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
    const FQN className = c.parseFQN();

    FQN parentName = {};
    AccessType inheretingPublicity = NONE;

    if (c.matchConsume(CbTokenType::PUNCTUATION_COLON))
    {
        if (c.matchConsume(CbTokenType::KEYWORD_TUTTI)) inheretingPublicity = PUBLIC;
        else if (c.matchConsume(CbTokenType::KEYWORD_SORDINO)) inheretingPublicity = PRIVATE;
        else if (c.matchConsume(CbTokenType::KEYWORD_SECTION)) inheretingPublicity = PROTECTED;
        else
        {
            c.addError(std::make_unique<InvalidAccessKeyword>(c.copyCurrent()));
            return nullptr;
        }
        parentName = c.parseFQN();
    }

    c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET);

    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<Ctor> ctors;

    while (!c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        bool isStatic = false;
        auto virtualType = VirtualType::NONE;
        AccessType access = PUBLIC;

        if (c.matchConsume(CbTokenType::KEYWORD_PLAYERSCORE)) access = PUBLIC;
        else if (c.matchConsume(CbTokenType::KEYWORD_CONDUCTORSCORE)) access = PRIVATE;
        else if (c.matchConsume(CbTokenType::KEYWORD_SECTIONSCORE)) access = PROTECTED;

        if (c.matchConsume(CbTokenType::KEYWORD_UNISON)) isStatic = true;
        if (c.matchConsume(CbTokenType::KEYWORD_VARIATION)) virtualType = VirtualType::OVERRIDE;
        if (c.matchConsume(CbTokenType::KEYWORD_REST)) virtualType = VirtualType::PURE;
        if (c.matchConsume(CbTokenType::KEYWORD_MOTIF)) virtualType = VirtualType::VIRTUAL;

        if (c.matchConsume(CbTokenType::KEYWORD_CTOR_CALL))
        {
            if (auto ctor = parseCtor(className)) ctors.emplace_back(access, std::move(ctor));
            if (!c.getIsInFunc())
            {
                break;
            }
        }
        else if (c.matchConsume(CbTokenType::KEYWORD_SONG))
        {
            if (auto method = parseFuncDeclStmt(true))
            {
                method->setIsStatic(isStatic);
                method->setVirtual(virtualType);
                methods.emplace_back(access, std::move(method));
            }
            if (!c.getIsInFunc())
            {
                break;
            }
        }
        else if (c.isType())
        {
            if (auto field = parseVarDecStmt())
            {
                field->setIsStatic(isStatic);
                fields.emplace_back(access, std::move(field));
            }
            if (!c.getIsInFunc())
            {
                break;
            }
        }
        else
        {
            c.addError(std::make_unique<UnexpectedToken>(c.copyCurrent()));
            c.advance();
        }
    }

    return std::make_unique<ClassDeclStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        className,
        fields,
        methods,
        ctors,
        inheretingPublicity,
        parentName,
        c.getClassDecl()
    );
}

std::unique_ptr<ConstractorDeclStmt> StmtParser::parseCtor(const FQN& className)
{
    if (const FQN ctorName = c.parseFQN(); ctorName != className)
    {
        c.addError(std::make_unique<InvalidCtorName>(c.copyCurrent(), translateFQNtoString(className)));
    }

    std::vector<Var> args;
    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN);
    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        const FQN argName = c.parseFQN();
        args.emplace_back(std::move(type), argName);
        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA);
    }

    auto stmt = std::make_unique<ConstractorDeclStmt>(
        c.copyCurrent(),
        className,
        args,
        c.getClassDecl()
    );

    stmt->setBody(parseBodyStmt(false, false, false, true));
    return stmt;
}

std::unique_ptr<ObjCreationStmt> StmtParser::parseObjCreationStmt() const
{
    auto type = typeParser.parseIType();
    const FQN name = c.parseFQN();

    std::vector<std::unique_ptr<Expr>> args;
    if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            args.push_back(exprParser.parseExpr());
            if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA);
        }
    }

    c.expect(CbTokenType::PUNCTUATION_SEMICOLON);
    auto ctorCall = std::make_unique<ConstractorCallStmt>(c.copyCurrent(), c.getFuncDecl(), std::move(args), c.getClassDecl());

    return std::make_unique<ObjCreationStmt>(
        c.copyCurrent(),
        c.getFuncDecl(),
        nullptr, 
        true,
        std::move(ctorCall),
        Var(std::move(type), name),
        c.getClassDecl()
    );
}
