#pragma once
#include "ParserContext.h"

class StmtParser
{
private:
    ParserContext& c;

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

public:
    explicit StmtParser(ParserContext& c);

    void parse() const;
};
