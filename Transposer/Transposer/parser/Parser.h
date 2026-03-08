#pragma once
#include <vector>
#include <queue>
#include <memory>

#include "ParserContext.h"
#include "errorHandling/Error.h"
#include "AST/abstract/Statement.h"
#include "AST/statements/ArrayDeclStmt.h"
#include "AST/statements/AssignmentStmt.h"
#include "AST/statements/BodyStmt.h"
#include "AST/statements/BreakStmt.h"
#include "AST/statements/CaseStmt.h"
#include "AST/statements/ClassDeclStmt.h"
#include "AST/statements/ContinueStmt.h"
#include "AST/statements/ForStmt.h"
#include "AST/statements/FuncCreditStmt.h"
#include "AST/statements/HearStmt.h"
#include "AST/statements/IfStmt.h"
#include "AST/statements/PlayStmt.h"
#include "AST/statements/VarDeclStmt.h"
#include "AST/statements/expression/UnaryOpExpr.h"
#include "AST/statements/ReturnStmt.h"
#include "AST/statements/SwitchStmt.h"
#include "AST/statements/WhileStmt.h"
#include "AST/statements/expression/ArrayIndexingExpr.h"
#include "AST/statements/expression/ArraySlicingExpr.h"
#include "AST/statements/expression/ConstValueExpr.h"
#include "AST/statements/expression/FuncCallExpr.h"
#include "other/SymbolTable.h"
#include "symbols/FuncCredit.h"
#include "symbols/Type/ArrayType.h"
#include "AST/statements/IncludeStmt.h"
#include "AST/statements/ObjCreationStmt.h"
#include "AST/statements/expression/DotOpExpr.h"
#include "AST/statements/expression/StaticDotOpExpr.h"
#include "symbols/Type/ClassType.h"

class Parser
{
private:
    ParserContext context;

    // General Statements

    // std::unique_ptr<VarDeclStmt> parseVarDecStmt(const bool isField = false);
    // std::unique_ptr<AssignmentStmt> parseAssignmentStmt();
    // std::unique_ptr<HearStmt> parseHearStmt();
    // std::unique_ptr<PlayStmt> parsePlayStmt();
    // std::unique_ptr<BodyStmt> parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal = false, const bool isBreakable = false, const bool isContinueAble = false, const bool hasBrace = true);
    // std::unique_ptr<FuncDeclStmt> parseFuncDeclStmt(const bool isMethod = false);
    // std::unique_ptr<ReturnStmt> parseReturnStmt();
    // std::unique_ptr<FuncCreditStmt> parseFuncCreditStmt();
    // std::unique_ptr<IfStmt> parseIfStmt();
    // std::unique_ptr<ArrayDeclStmt> parseArrayDeclStmt();
    // std::unique_ptr<WhileStmt> parseWhileStmt();
    // std::unique_ptr<BreakStmt> parseBreakStmt();
    // std::unique_ptr<CaseStmt> parseCaseStmt();
    // std::unique_ptr<SwitchStmt> parseSwitchStmt();
    // std::unique_ptr<ContinueStmt> parseContinueStmt();
    // std::unique_ptr<ForStmt> parseForStmt();
    // std::unique_ptr<ClassDeclStmt> parseClassDeclStmt();
    // std::unique_ptr<ConstractorDeclStmt> parseCtor();
    // std::unique_ptr<ConstractorCallStmt> parseConstractorCallStmt();
    // std::unique_ptr<ObjCreationStmt> parseObjCreationStmt();
    // bool parseCtorCall(const ClassNode* c);
    // bool parseFields(std::vector<Field>& fields);
    // bool parseMethods(std::vector<Method>& methods);
    // bool parseCtors(std::vector<Ctor>& ctors);
    //
    // // Expressions
    //
    // std::unique_ptr<Expr> parseExpr(const bool hasParens = false, const bool isStmt = false, const bool allowBackslash = true);
    // std::unique_ptr<Expr> parsePrimary(const bool isStmt = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    // std::unique_ptr<Expr> parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left, const bool isStmt = false, const bool allowBackslash = true, const ClassNode* classCall = nullptr);
    // std::unique_ptr<StaticDotOpExpr> parseStaticDotOpExpr(const bool isStmt = false);
    // std::unique_ptr<ConstValueExpr> parseConstValueExpr();
    // std::unique_ptr<UnaryOpExpr> parseUnaryOpExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseVarCallExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseFuncCallExpr(const bool isStmt = false);
    // std::unique_ptr<Call> parseCallExpr(const ClassNode* classCall = nullptr);
    // std::unique_ptr<Call> parseArrayAccess(std::unique_ptr<Call> call);
    // std::unique_ptr<Call> parseArraySlicingExpr(std::unique_ptr<Call> call);
    // std::unique_ptr<Call> parseArrayIndexingExpr(std::unique_ptr<Call> call);
    //
    // // Types
    //
    // std::unique_ptr<IType> parseIType();
    // std::unique_ptr<Type> parseType();
    // std::unique_ptr<ArrayType> parseArrayType();
    // std::unique_ptr<ClassType> parseClassType();

public:
    explicit Parser(const std::vector<Token>& tokens);

    const ParserContext& getContext() const;
    ParserContext& getContext();

    std::vector<std::pair<std::filesystem::path, Token>> readIncludes();
    void parse();
    void analyze();
    std::string translateToCpp(const std::filesystem::path& hPath, const bool isMain = false) const;
    std::string translateToH(const bool isMain = false) const;
    bool shouldProduceCpp(bool isMain) const;
};

