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
#include "AST/statements/ConstructorDeclStmt.h"
#include "AST/statements/ConstractorCallStmt.h"
#include "AST/statements/ObjCreationStmt.h"

#include "errorHandling/syntaxErrors/UnexpectedToken.h"
#include "errorHandling/syntaxErrors/MissingBrace.h"
#include "errorHandling/syntaxErrors/MissingIdentifier.h"
#include "errorHandling/syntaxErrors/MissingParenthesis.h"
#include "errorHandling/syntaxErrors/ExpectedKeywordNotFound.h"
#include "errorHandling/syntaxErrors/ExpectedAnExpression.h"
#include "errorHandling/syntaxErrors/IncludeNotInTop.h"
#include "errorHandling/syntaxErrors/InvalidExpression.h"
#include "errorHandling/syntaxErrors/NoPlacementOperator.h"
#include "errorHandling/semanticErrors/StmtNotBreakable.h"
#include "errorHandling/semanticErrors/StmtNotContinueAble.h"

#include "errorHandling/classErrors/InvalidCtorName.h"

#include "errorHandling/lexicalErrors/UnrecognizedToken.h"

#include "errorHandling/how/HowDareYou.h"

// Helper to determine if a statement consumes its own trailing semicolon/terminator
static bool isSelfTerminating(const Stmt* stmt)
{
    return dynamic_cast<const IfStmt*>(stmt) != nullptr ||
           dynamic_cast<const ForStmt*>(stmt) != nullptr ||
           dynamic_cast<const WhileStmt*>(stmt) != nullptr ||
           dynamic_cast<const SwitchStmt*>(stmt) != nullptr ||
           dynamic_cast<const ClassDeclStmt*>(stmt) != nullptr ||
           dynamic_cast<const FuncDeclStmt*>(stmt) != nullptr ||
           dynamic_cast<const BodyStmt*>(stmt) != nullptr;
}

StmtParser::StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser)
    : c(c), typeParser(typeParser), exprParser(exprParser)
{
}

std::unique_ptr<Stmt> StmtParser::parseStmt()
{
    while (c.matchConsume(CbTokenType::PUNCTUATION_NEW_LINE)) {} // if new lines got here they are in the global scope and we can just eat them

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
        auto res = exprParser.parseExpr();
        if (res) res->setNeedsSemicolon(true);
        return res;
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
        if (!c.getIsBreakable())
        {
            c.addError(std::make_unique<StmtNotBreakable>(c.copyCurrent()));
        }
        return std::make_unique<BreakStmt>(c.copyCurrent());
    }
    if (c.matchConsume(CbTokenType::KEYWORD_RESUME))
    {
        if (!c.getIsContinueable())
        {
            c.addError(std::make_unique<StmtNotContinueAble>(c.copyCurrent()));
        }
        return std::make_unique<ContinueStmt>(c.copyCurrent());
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
        init != nullptr,
        std::move(init),
        Var(std::move(type), name)
    );

    return stmt;
}

std::unique_ptr<AssignmentStmt> StmtParser::parseAssignmentStmt(std::unique_ptr<VarReference> left) const
{
    if (!c.isBinaryOp())
    {
        c.addError(std::make_unique<NoPlacementOperator>(c.copyCurrent()));
        return nullptr;
    }
    Token opToken;
    c.matchConsume(c.current().type, opToken);

    auto right = exprParser.parseExpr();
    if (!right)
        return nullptr;

    return std::make_unique<AssignmentStmt>(
        opToken,
        std::move(left),
        opToken.value.value(),
        std::move(right)
    );
}

std::unique_ptr<HearStmt> StmtParser::parseHearStmt() const
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

    std::vector<std::unique_ptr<VarReference>> calls;

    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto var = exprParser.parseVarExpr();
        calls.push_back(std::move(var));

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.copyCurrent()));
        }
    }

    return std::make_unique<HearStmt>(
        c.copyCurrent(),
        calls
    );
}

std::unique_ptr<PlayStmt> StmtParser::parsePlayStmt() const
{
    Token t;
    bool isBar = false;
    if (c.matchConsume(CbTokenType::KEYWORD_PLAYBAR, t)) isBar = true;
    else c.matchConsume(CbTokenType::KEYWORD_PLAY, t);

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

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
        std::move(exprs),
        isBar
    );
}

std::unique_ptr<BodyStmt> StmtParser::parseBodyStmt(const bool isGlobal, const bool hasBrace)
{
    Token t = c.copyCurrent();
    if (hasBrace)
    {
        if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.copyCurrent())))
            return nullptr;
    }

    std::vector<std::unique_ptr<Stmt>> bodyStmts;
    while (!hasBrace || !c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        auto stmt = parseStmt();
        if (stmt)
        {
            const bool selfTerminating = isSelfTerminating(stmt.get());
            bodyStmts.push_back(std::move(stmt));
            if (!selfTerminating)
            {
                c.expectSemiColon();
            }
        }

        if (!c.getIsInFunc() && !hasBrace)
        {
            break;
        }
        
        if (c.isEmpty()) break;
    }

    return std::make_unique<BodyStmt>(
        t,
        bodyStmts,
        isGlobal
    );
}

std::unique_ptr<FuncDeclStmt> StmtParser::parseFuncDeclStmt(const bool isMethod)
{

    std::vector<std::unique_ptr<FuncCreditStmt>> credited;

    if (c.matchConsume(CbTokenType::PUNCTUATION_COPYRIGHT))
    {
        if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        {
            return nullptr;
        }

        while (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (auto credit = parseFuncCreditStmt()) credited.push_back(std::move(credit));
            if (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.copyCurrent()));
        }
    }

    const FQN name = c.parseFQN();

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

    c.setIsInFunc(true);

    std::vector<Var> args;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        if (!type)
            return nullptr;

        const FQN argName = c.parseFQN();
        args.emplace_back(std::move(type), argName);

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (!c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.copyCurrent())))
                return nullptr;
        }
    }

    std::unique_ptr<IType> retType = nullptr;
    if (c.matchConsume(CbTokenType::PUNCTUATION_RET_TYPE_ARROW))
    {
        retType = typeParser.parseIType();
    }
    else
    {
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
        false
    );

    funcStmt->setBody(parseBodyStmt(false, false));

    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt() const
{
    std::unique_ptr<Expr> expr = nullptr;
    if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
        expr = exprParser.parseExpr();

    return std::make_unique<ReturnStmt>(
        c.copyCurrent(),
        expr,
        nullptr
    );
}

std::unique_ptr<FuncCreditStmt> StmtParser::parseFuncCreditStmt() const
{
    Token t = c.copyCurrent();
    const FQN name = c.parseFQN();

    return std::make_unique<FuncCreditStmt>(
        t,
        FuncCredit(name, t)
    );
}

StmtWithBody StmtParser::parseConditionAndBody()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN,
                  std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return {nullptr, nullptr};

    auto condition = exprParser.parseExpr();
    if (!condition)
    {
        c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
        return {nullptr, nullptr};
    }

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE,
                  std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return {nullptr, nullptr};

    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();

    return {std::move(condition), std::move(body)};
}

StmtWithBody StmtParser::parseElseBody()
{
    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();
    return {nullptr, std::move(body)};
}

std::unique_ptr<IfStmt> StmtParser::parseIfStmt()
{
    auto currStmt = parseConditionAndBody();
    if (!currStmt.expr && !currStmt.body)
        return nullptr;

    std::vector<StmtWithBody> elses;
    while (c.matchConsume(CbTokenType::KEYWORD_ELSE))
    {
        if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
        {
            if (!c.expect(CbTokenType::KEYWORD_IF,
                          std::make_unique<ExpectedKeywordNotFound>(c.copyCurrent(), "D")))
                break;

            auto elseIf = parseConditionAndBody();
            if (!elseIf.expr && !elseIf.body)
                break;

            elses.push_back(std::move(elseIf));
        }
        else
        {
            elses.push_back(parseElseBody());
            break;
        }
    }

    return std::make_unique<IfStmt>(
        c.copyCurrent(),
        std::move(currStmt),
        elses
    );
}

std::unique_ptr<WhileStmt> StmtParser::parseWhileStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

    auto condition = exprParser.parseExpr();

    if (!condition)
        return nullptr;

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

    c.addBreakable();
    c.addContinueable();

    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();
    
    auto stmt = StmtWithBody(std::move(condition), std::move(body));

    c.removeBreakable();
    c.removeContinueable();

    return std::make_unique<WhileStmt>(
        c.copyCurrent(),
        std::move(stmt)
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;

    auto expr = exprParser.parseExpr();
    if (!expr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.copyCurrent()));
        return nullptr;
    }


    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.copyCurrent())))
        return nullptr;
    if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.copyCurrent())))
        return nullptr;

    c.addBreakable();

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.matchConsume(CbTokenType::KEYWORD_CASE))
        {
            if (auto cs = parseCaseStmt()) cases.push_back(std::move(cs));
        }
        else c.advance();
    }

    c.removeBreakable();

    return std::make_unique<SwitchStmt>(
        c.copyCurrent(),
        std::move(expr),
        cases
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

    auto body = parseBodyStmt(false, false);
    StmtWithBody ret(std::move(caseing), std::move(body));

    return std::make_unique<CaseStmt>(
        c.copyCurrent(),
        std::move(ret),
        isDefault
    );
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt() const
{
    Token t = c.copyCurrent();
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

    return std::make_unique<ArrayDeclStmt>(
        t,
        false,
        nullptr,
        Var(std::move(type), name),
        std::move(sizes)
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
    if (c.matchConsume(CbTokenType::UNARY_OP_DOUBLE_SHARP) || c.matchConsume(CbTokenType::UNARY_OP_DOUBLE_FLAT))
    {
        stopExpr = exprParser.parseExpr();
    }

    Token varToken;
    if (c.matchConsume(CbTokenType::PUNCTUATION_DOUBLE_BACKSLASH))
        c.expect(CbTokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.copyCurrent()), varToken);

    c.addBreakable();
    c.addContinueable();

    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();

    c.removeBreakable();
    c.removeContinueable();

    return std::make_unique<ForStmt>(
        c.copyCurrent(),
        std::move(body),
        isIncreasing,
        std::move(startExpr),
        std::move(stepExpr),
        std::move(stopExpr),
        varToken.value.value_or("i")
    );
}

std::unique_ptr<ClassDeclStmt> StmtParser::parseClassDeclStmt()
{
    const FQN className = c.parseFQN();

    FQN parentName = {};

    if (c.matchConsume(CbTokenType::PUNCTUATION_COLON))
    {
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
        AccessType access = PRIVATE;

        if (c.matchConsume(CbTokenType::KEYWORD_PLAYERSCORE)) access = PUBLIC;
        else if (c.matchConsume(CbTokenType::KEYWORD_CONDUCTORSCORE)) access = PRIVATE;
        else if (c.matchConsume(CbTokenType::KEYWORD_SECTIONSCORE)) access = PROTECTED;

        if (c.matchConsume(CbTokenType::KEYWORD_UNISON)) isStatic = true;
        else if (c.matchConsume(CbTokenType::KEYWORD_VARIATION)) virtualType = VirtualType::OVERRIDE;
        else if (c.matchConsume(CbTokenType::KEYWORD_REST)) virtualType = VirtualType::PURE;
        else if (c.matchConsume(CbTokenType::KEYWORD_MOTIF)) virtualType = VirtualType::VIRTUAL;

        if (c.matchConsume(CbTokenType::KEYWORD_CTOR_CALL))
        {
            if (auto ctor = parseCtor(className)) ctors.emplace_back(access, std::move(ctor));
            c.expectSemiColon();
        }
        else if (c.matchConsume(CbTokenType::KEYWORD_SONG))
        {
            if (auto method = parseFuncDeclStmt(true))
            {
                method->setIsStatic(isStatic);
                method->setVirtual(virtualType);
                methods.emplace_back(access, std::move(method));
                c.expectSemiColon();
            }
        }
        else if (c.isType())
        {
            if (auto field = parseVarDecStmt())
            {
                field->setIsStatic(isStatic);
                fields.emplace_back(access, std::move(field));
                c.expectSemiColon();
            }
        }
        else
        {
            c.addError(std::make_unique<UnexpectedToken>(c.copyCurrent()));
            c.advance();
        }
    }

    c.expectSemiColon();

    return std::make_unique<ClassDeclStmt>(
        c.copyCurrent(),
        className,
        fields,
        methods,
        ctors,
        parentName
    );
}

std::unique_ptr<ConstructorDeclStmt> StmtParser::parseCtor(const FQN& className)
{
    if (const FQN ctorName = c.parseFQN(); ctorName != className)
    {
        c.addError(std::make_unique<InvalidCtorName>(c.copyCurrent(), translateFQNtoString(className)));
    }

    c.setIsInFunc(true);
    std::vector<Var> args;
    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN);
    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto type = typeParser.parseIType();
        const FQN argName = c.parseFQN();
        args.emplace_back(std::move(type), argName);
        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA);
    }

    auto stmt = std::make_unique<ConstructorDeclStmt>(
        c.copyCurrent(),
        className,
        args
    );

    stmt->setBody(parseBodyStmt(false, false));
    return stmt;
}

std::unique_ptr<ObjCreationStmt> StmtParser::parseObjCreationStmt() const
{
    auto type = typeParser.parseIType();
    const FQN name = c.parseFQN();
    bool hasCtorArgs;

    std::vector<std::unique_ptr<Expr>> args;
    if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        hasCtorArgs = true;
        while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            args.push_back(exprParser.parseExpr());
            if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA);
        }
    }

    auto ctorCall = std::make_unique<ConstractorCallStmt>(c.copyCurrent(), std::move(args));

    return std::make_unique<ObjCreationStmt>(
        c.copyCurrent(),
        nullptr,
        hasCtorArgs,
        std::move(ctorCall),
        Var(std::move(type), name)
    );
}

void StmtParser::parse()
{
    while (!c.isEmpty())
    {
        if (auto stmt = parseStmt())
        {
            const bool selfTerm = isSelfTerminating(stmt.get());
            c.getStmts().push_back(std::move(stmt));
            if (!selfTerm)
            {
                c.expectSemiColon();
            }
        }
        else
        {
            c.advance();
        }
    }
}
