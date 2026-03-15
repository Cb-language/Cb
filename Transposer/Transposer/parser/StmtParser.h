#pragma once

#include "ParserContext.h"

#include "AST/statements/ArrayDeclStmt.h"
#include "AST/statements/AssignmentStmt.h"
#include "AST/statements/BreakStmt.h"
#include "AST/statements/CaseStmt.h"
#include "AST/statements/ContinueStmt.h"
#include "AST/statements/ForStmt.h"
#include "AST/statements/HearStmt.h"
#include "AST/statements/IfStmt.h"
#include "AST/statements/ObjCreationPolyStmt.h"
#include "AST/statements/PlayStmt.h"
#include "AST/statements/ReturnStmt.h"
#include "AST/statements/SwitchStmt.h"
#include "AST/statements/WhileStmt.h"

class TypeParser;
class ExprParser;


class StmtParser
{
private:
    ParserContext& c;
    TypeParser& typeParser;
    ExprParser& exprParser;

public:
    StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser);

    std::unique_ptr<Stmt> parseStmt();

    std::unique_ptr<VarDeclStmt> parseVarDecStmt() const;
    std::unique_ptr<AssignmentStmt> parseAssignmentStmt(std::unique_ptr<VarReference> left) const;

    std::unique_ptr<HearStmt> parseHearStmt() const;
    std::unique_ptr<PlayStmt> parsePlayStmt() const;

    std::unique_ptr<BodyStmt> parseBodyStmt(bool isGlobal = false, bool hasBrace = true);
    std::unique_ptr<FuncDeclStmt> parseFuncDeclStmt(const bool isMethod = false);
    std::unique_ptr<ReturnStmt> parseReturnStmt() const;
    std::unique_ptr<FuncCreditStmt> parseFuncCreditStmt() const;

    StmtWithBody parseConditionAndBody();
    StmtWithBody parseElseBody();
    std::unique_ptr<IfStmt> parseIfStmt();
    std::unique_ptr<WhileStmt> parseWhileStmt();
    std::unique_ptr<SwitchStmt> parseSwitchStmt();
    std::unique_ptr<CaseStmt> parseCaseStmt();
    std::unique_ptr<ArrayDeclStmt> parseArrayDeclStmt() const;
    std::unique_ptr<ForStmt> parseForStmt();

    std::unique_ptr<ClassDeclStmt> parseClassDeclStmt();
    std::unique_ptr<ConstructorDeclStmt> parseCtor(const FQN& className);
    std::unique_ptr<ObjectCreationStmt> parseObjCreationStmt() const;
    std::unique_ptr<ObjectCreationStmt> parsePolyObjCreationStmt(std::unique_ptr<IType> type) const;

    void parse();
};
