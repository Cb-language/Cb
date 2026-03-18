#include "StmtParser.h"

#include <algorithm>

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
#include "AST/statements/ContinueStmt.h"
#include "AST/statements/ObjCreationCastStmt.h"
#include "AST/statements/ObjCreationPolyStmt.h"

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
#include "errorHandling/syntaxErrors/CaseNotInsideIf.h"
#include "errorHandling/syntaxErrors/ElseWithoutIf.h"
#include "symbols/Type/ClassType.h"

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
        throw HowDareYou(c.getLastToken());
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

        if (c.matchNonConsume(CbTokenType::IDENTIFIER))
        {
            if (const auto varRef = dynamic_cast<VarCallExpr*>(res.get()))
            {
                std::unique_ptr<IType> type = std::make_unique<ClassType>(varRef->getName());
                return parsePolyObjCreationStmt(std::move(type));
            }
        }
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
            c.addError(std::make_unique<StmtNotBreakable>(c.getLastToken()));
        }
        return std::make_unique<BreakStmt>(c.getLastToken());
    }
    if (c.matchConsume(CbTokenType::KEYWORD_RESUME))
    {
        if (!c.getIsContinueable())
        {
            c.addError(std::make_unique<StmtNotContinueAble>(c.getLastToken()));
        }
        return std::make_unique<ContinueStmt>(c.getLastToken());
    }
    if (c.matchConsume(CbTokenType::KEYWORD_FEAT))
    {
        c.addError(std::make_unique<IncludeNotInTop>(c.getLastToken()));
        return nullptr;
    }
    if (c.matchConsume(CbTokenType::KEYWORD_ELSE))
    {
        c.addError(std::make_unique<ElseWithoutIf>(c.getLastToken()));
        return nullptr;
    }
    if (c.matchConsume(CbTokenType::KEYWORD_CASE))
    {
        std::make_unique<CaseNotInsideIf>(c.getLastToken());
    }

    c.addError(std::make_unique<UnrecognizedToken>(c.advance()));
    return nullptr;
}

std::unique_ptr<VarDeclStmt> StmtParser::parseVarDecStmt() const
{
    auto type = typeParser.parseIType();
    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(c.getLastToken()));
        return nullptr;
    }

    const FQN name = c.parseFQN();

    std::unique_ptr<Expr> init = nullptr;
    if (c.matchConsume(CbTokenType::BINARY_OP_EQUAL))
    {
        init = exprParser.parseExpr();

        if (init == nullptr)
        {
            c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
            return nullptr;
        }
    }

    auto stmt = std::make_unique<VarDeclStmt>(
        c.getLastToken(),
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
        c.addError(std::make_unique<NoPlacementOperator>(c.getLastToken()));
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
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;

    std::vector<std::unique_ptr<VarReference>> calls;

    while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
    {
        auto var = exprParser.parseVarExpr();
        calls.push_back(std::move(var));

        if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.getLastToken()));
        }
    }

    return std::make_unique<HearStmt>(
        c.getLastToken(),
        calls
    );
}

std::unique_ptr<PlayStmt> StmtParser::parsePlayStmt() const
{
    Token t;
    bool isBar = false;
    if (c.matchConsume(CbTokenType::KEYWORD_PLAYBAR, t)) isBar = true;
    else c.matchConsume(CbTokenType::KEYWORD_PLAY, t);

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
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
            c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<InvalidExpression>(c.getLastToken()));
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
    if (hasBrace)
    {
        if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.getLastToken())))
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
        c.getLastToken(),
        bodyStmts,
        isGlobal
    );
}

std::unique_ptr<FuncDeclStmt> StmtParser::parseFuncDeclStmt(const bool isMethod, const VirtualType vType)
{

    std::vector<std::unique_ptr<FuncCreditStmt>> credited;

    if (c.matchConsume(CbTokenType::PUNCTUATION_COPYRIGHT))
    {
        if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
        {
            return nullptr;
        }

        while (!c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE))
        {
            if (auto credit = parseFuncCreditStmt()) credited.push_back(std::move(credit));
            if (!c.matchNonConsume(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE)) c.expect(CbTokenType::PUNCTUATION_COMMA, std::make_unique<UnexpectedToken>(c.getLastToken()));
        }
    }

    const FQN name = c.parseFQN();

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;

    if (vType != VirtualType::PURE)
    {
        c.setIsInFunc(true);
    }

    std::vector<Var> args = exprParser.getArgsWithTypes();

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
        c.getLastToken(),
        name,
        std::move(retType),
        args,
        credited,
        isMethod,
        VirtualType::NONE,
        false
    );

    if (vType == VirtualType::PURE)
    {
        funcStmt->setBody(nullptr);
    }
    else
    {
        funcStmt->setBody(parseBodyStmt(false, false));
    }

    return funcStmt;
}

std::unique_ptr<ReturnStmt> StmtParser::parseReturnStmt() const
{
    std::unique_ptr<Expr> expr = nullptr;
    if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
        expr = exprParser.parseExpr();

    return std::make_unique<ReturnStmt>(
        c.getLastToken(),
        expr,
        nullptr
    );
}

std::unique_ptr<FuncCreditStmt> StmtParser::parseFuncCreditStmt() const
{
    const FQN name = c.parseFQN();

    return std::make_unique<FuncCreditStmt>(
        c.getLastToken(),
        FuncCredit(name, c.getLastToken())
    );
}

StmtWithBody StmtParser::parseConditionAndBody()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN,
                  std::make_unique<MissingParenthesis>(c.getLastToken())))
        return {nullptr, nullptr};

    auto condition = exprParser.parseExpr();
    if (!condition)
    {
        c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
        return {nullptr, nullptr};
    }

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE,
                  std::make_unique<MissingParenthesis>(c.getLastToken())))
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
                          std::make_unique<ExpectedKeywordNotFound>(c.getLastToken(), "D")))
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
        c.getLastToken(),
        std::move(currStmt),
        elses
    );
}

std::unique_ptr<WhileStmt> StmtParser::parseWhileStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;

    auto condition = exprParser.parseExpr();

    if (!condition)
        return nullptr;

    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;

    c.addBreakable();
    c.addContinueable();

    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();
    
    auto stmt = StmtWithBody(std::move(condition), std::move(body));

    c.removeBreakable();
    c.removeContinueable();

    return std::make_unique<WhileStmt>(
        c.getLastToken(),
        std::move(stmt)
    );
}

std::unique_ptr<SwitchStmt> StmtParser::parseSwitchStmt()
{
    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;

    auto expr = exprParser.parseExpr();
    if (!expr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
        return nullptr;
    }


    if (!c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_CLOSE, std::make_unique<MissingParenthesis>(c.getLastToken())))
        return nullptr;
    if (!c.expect(CbTokenType::PUNCTUATION_OPEN_CURLY_BRACKET, std::make_unique<MissingBrace>(c.getLastToken())))
        return nullptr;

    c.addBreakable();

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!c.matchConsume(CbTokenType::PUNCTUATION_CLOSE_CURLY_BRACKET))
    {
        if (c.matchConsume(CbTokenType::KEYWORD_CASE))
        {
            if (auto cs = parseCaseStmt()) cases.push_back(std::move(cs));
        }
        else
        {
            c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
            c.advance();
        }
    }

    c.removeBreakable();

    return std::make_unique<SwitchStmt>(
        c.getLastToken(),
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
            c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
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
        c.getLastToken(),
        std::move(ret),
        isDefault
    );
}

std::unique_ptr<ArrayDeclStmt> StmtParser::parseArrayDeclStmt() const
{
    auto type = typeParser.parseIType();

    if (!type)
    {
        c.addError(std::make_unique<UnrecognizedToken>(c.getLastToken()));
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
        c.getLastToken(),
        false,
        nullptr,
        Var(std::move(type), name),
        std::move(sizes)
    );
}

std::unique_ptr<ForStmt> StmtParser::parseForStmt()
{
    bool isIncreasing;

    if (c.matchConsume(CbTokenType::KEYWORD_FMAJ)) isIncreasing = true;
    else if (c.matchConsume(CbTokenType::KEYWORD_FMIN)) isIncreasing = false;
    else return nullptr;

    std::unique_ptr<Expr> startExpr = exprParser.parseExpr();
    if (!startExpr)
    {
        c.addError(std::make_unique<InvalidExpression>(c.getLastToken()));
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
        c.expect(CbTokenType::IDENTIFIER, std::make_unique<MissingIdentifier>(c.getLastToken()), varToken);

    c.addBreakable();
    c.addContinueable();

    auto body = parseBodyStmt(false, true);
    c.expectSemiColon();

    c.removeBreakable();
    c.removeContinueable();

    return std::make_unique<ForStmt>(
        c.getLastToken(),
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
        AccessType access = PUBLIC; // default public

        if (c.matchConsume(CbTokenType::KEYWORD_PLAYERSCORE)) access = PRIVATE;
        else if (c.matchConsume(CbTokenType::KEYWORD_CONDUCTORSCORE)) access = PUBLIC;
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
            if (auto method = parseFuncDeclStmt(true, virtualType))
            {
                method->setIsStatic(isStatic);
                method->setVirtual(virtualType);
                methods.emplace_back(access, std::move(method));
                c.expectSemiColon();
            }
        }
        else if (c.isType())
        {
            if (c.matchNonConsume(CbTokenType::TYPE_RIFF))
            {
                if (auto field = parseArrayDeclStmt())
                {
                    field->setIsStatic(isStatic);
                    fields.emplace_back(access, std::move(field));
                    c.expectSemiColon();
                }
            }
            else if (auto field = parseVarDecStmt())
            {
                field->setIsStatic(isStatic);
                fields.emplace_back(access, std::move(field));
                c.expectSemiColon();
            }
        }
        else
        {
            c.addError(std::make_unique<UnexpectedToken>(c.getLastToken()));
            c.advance();
        }
    }

    c.expectSemiColon();

    return std::make_unique<ClassDeclStmt>(
        c.getLastToken(),
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
        c.addError(std::make_unique<InvalidCtorName>(c.getLastToken(), translateFQNtoString(className)));
    }

    c.setIsInFunc(true);
    c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN, std::make_unique<MissingBrace>(c.copyCurrent()));
    std::vector<Var> args = exprParser.getArgsWithTypes();

    auto stmt = std::make_unique<ConstructorDeclStmt>(
        c.getLastToken(),
        className,
        args
    );

    if (c.matchConsume(CbTokenType::PUNCTUATION_BACKSLASH))
    {
        c.expect(CbTokenType::KEYWORD_BASS);
        c.expect(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN);
        std::vector<std::unique_ptr<Expr>> parentArgs = exprParser.getArgsWithoutTypes();
        stmt->setParentCtorCall(std::move(parentArgs));
    }
    if (c.getIsInFunc())
    {
        stmt->setBody(parseBodyStmt(false, false));
    }
    else
    {
        std::vector<std::unique_ptr<Stmt>> empty;
        stmt->setBody(std::make_unique<BodyStmt>(c.copyCurrent(), empty));
    }
    return stmt;
}

std::unique_ptr<ObjectCreationStmt> StmtParser::parseObjCreationStmt() const
{
    auto type = typeParser.parseIType();
    const FQN name = c.parseFQN();
    bool hasCtorArgs;

    std::vector<std::unique_ptr<Expr>> args;
    if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
    {
        hasCtorArgs = true;
        args = exprParser.getArgsWithoutTypes();
    }

    auto ctorCall = std::make_unique<ConstractorCallStmt>(c.getLastToken(), std::move(args));

    return std::make_unique<ObjCreationPolyStmt>(
        c.getLastToken(),
        nullptr,
        hasCtorArgs,
        std::move(ctorCall),
        Var(std::move(type), name)
    );
}

std::unique_ptr<ObjectCreationStmt> StmtParser::parsePolyObjCreationStmt(std::unique_ptr<IType> type) const
{
    const FQN name = c.parseFQN();

    c.expect(CbTokenType::BINARY_OP_EQUAL);
    if (c.matchConsume(CbTokenType::KEYWORD_CTOR_CALL))
    {
        const FQN ctorName = c.parseFQN();

        bool hasCtorArgs;
        std::vector<std::unique_ptr<Expr>> args;
        if (c.matchConsume(CbTokenType::PUNCTUATION_PARENTHESIS_OPEN))
        {
            hasCtorArgs = true;
            args = exprParser.getArgsWithoutTypes();
        }

        auto ctorCall = std::make_unique<ConstractorCallStmt>(c.getLastToken(), std::move(args));

        return std::make_unique<ObjCreationPolyStmt>(
            c.getLastToken(),
            nullptr,
            hasCtorArgs,
            std::move(ctorCall),
            Var(std::move(type), name),
            ctorName
        );
    }

    c.expect(CbTokenType::KEYWORD_TRANSCRIBE);

    if (c.matchConsume(CbTokenType::PUNCTUATION_SEMICOLON))
    {
        return std::make_unique<ObjCreationCastStmt>(c.getLastToken(), false, nullptr, Var(std::move(type), name));
    }

    c.expect(CbTokenType::BINARY_OP_EQUAL);
    std::unique_ptr<CastCallExpr> right = exprParser.parseCastExpr();

    return std::make_unique<ObjCreationCastStmt>(c.getLastToken(), true, std::move(right), Var(std::move(type), name));
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
