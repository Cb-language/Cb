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
#include "AST/statements/ObjCreationStmt.h"
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
    explicit StmtParser(ParserContext& c, TypeParser& typeParser, ExprParser& exprParser);

    bool expectSemiColon() const;

    std::unique_ptr<Stmt> parseStmt(const bool isGlobal = false, const bool isBreakable = false, const bool isContinueAble = false);
    std::unique_ptr<VarDeclStmt> parseVarDecStmt(const bool isField = false) const;
    std::unique_ptr<AssignmentStmt> parseAssignmentStmt() const;
    std::unique_ptr<HearStmt> parseHearStmt();
    std::unique_ptr<PlayStmt> parsePlayStmt();
    std::unique_ptr<BodyStmt> parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal = false, const bool isBreakable = false, const bool isContinueAble = false, const bool hasBrace = true);
    std::unique_ptr<FuncDeclStmt> parseFuncDeclStmt(const bool isMethod = false);
    std::unique_ptr<ReturnStmt> parseReturnStmt();
    std::unique_ptr<FuncCreditStmt> parseFuncCreditStmt();
    std::unique_ptr<IfStmt> parseIfStmt();
    std::unique_ptr<ArrayDeclStmt> parseArrayDeclStmt();
    std::unique_ptr<WhileStmt> parseWhileStmt();
    std::unique_ptr<BreakStmt> parseBreakStmt();
    std::unique_ptr<CaseStmt> parseCaseStmt();
    std::unique_ptr<SwitchStmt> parseSwitchStmt();
    std::unique_ptr<ContinueStmt> parseContinueStmt();
    std::unique_ptr<ForStmt> parseForStmt();
    std::unique_ptr<ClassDeclStmt> parseClassDeclStmt();
    std::unique_ptr<ConstractorDeclStmt> parseCtor();
    std::unique_ptr<ConstractorCallStmt> parseConstractorCallStmt();
    std::unique_ptr<ObjCreationStmt> parseObjCreationStmt();
    bool parseFields(std::vector<Field>& fields);
    bool parseMethods(std::vector<Method>& methods);
    bool parseCtors(std::vector<Ctor>& ctors);

    void parse();
};
