// #include "Parser.h"
//
// #include <memory>
// #include <ranges>
// #include <sstream>
// #include <vector>
// #include <filesystem>
// #include <algorithm>
//
// // ---------- AST related ----------
// #include "AST/statements/FuncDeclStmt.h"
// #include "AST/statements/expression/BinaryOpExpr.h"
// #include "AST/statements/expression/FuncCallExpr.h"
//
// // ---------- syntax errors ----------
// #include "../errorHandling/syntaxErrors/UnexpectedToken.h"
// #include "../errorHandling/syntaxErrors/ExpectedAnExpression.h"
// #include "../errorHandling/syntaxErrors/MissingBrace.h"
// #include "../errorHandling/syntaxErrors/MissingPipe.h"
// #include "../errorHandling/syntaxErrors/MissingSemicolon.h"
// #include "../errorHandling/syntaxErrors/NoPlacementOperator.h"
// #include "../errorHandling/syntaxErrors/InvalidExpression.h"
// #include "../errorHandling/syntaxErrors/StmtInsideSwitchThatIsNotCase.h"
// #include "../errorHandling/syntaxErrors/MissingIdentifier.h"
// #include "../errorHandling/syntaxErrors/MissingParenthesis.h"
// #include "../errorHandling/syntaxErrors/IncludeNotInTop.h"
// #include "../errorHandling/syntaxErrors/ExpectedAPath.h"
// #include "../errorHandling/syntaxErrors/IllegalVarName.h"
// #include "../errorHandling/syntaxErrors/ExpectedKeywordNotFound.h"
//
// // ---------- semantic errors ----------
// #include "../errorHandling/semanticErrors/IdentifierTaken.h"
// #include "../errorHandling/semanticErrors/StmtNotBreakable.h"
// #include "../errorHandling/semanticErrors/StmtNotContinueAble.h"
// #include "../errorHandling/semanticErrors/NoReturn.h"
// #include "../errorHandling/semanticErrors/WrongReturnType.h"
// #include "../errorHandling/semanticErrors/UnrecognizedIdentifier.h"
// #include "../errorHandling/semanticErrors/IllegalCredit.h"
// #include "../errorHandling/semanticErrors/IllegalCall.h"
// #include "../errorHandling/semanticErrors/InvalidPathExtension.h"
// #include "../errorHandling/semanticErrors/StaticFuncCantVirtual.h"
//
// // ---------- entry point errors ----------
// #include "../errorHandling/entryPointErrors/InvalidMainReturnType.h"
// #include "../errorHandling/entryPointErrors/InvalidMainArgs.h"
// #include "../errorHandling/entryPointErrors/MainOverride.h"
//
// // ---------- lexical errors ----------
// #include "../errorHandling/lexicalErrors/InvalidNumberLiteral.h"
// #include "../errorHandling/lexicalErrors/UnexpectedEOF.h"
// #include "../errorHandling/lexicalErrors/UnrecognizedToken.h"
//
// // ---------- class errors ----------
// #include "../errorHandling/classErrors/ClassDosentExisit.h"
// #include "../errorHandling/classErrors/IllegalFieldName.h"
// #include "../errorHandling/classErrors/InvalidAccessKeyword.h"
// #include "../errorHandling/classErrors/MissingClassPipe.h"
// #include "../errorHandling/classErrors/NoCtor.h"
// #include "../errorHandling/classErrors/RedefOfCtor.h"
// #include "../errorHandling/classErrors/InvalidOverrideSignature.h"
// #include "../errorHandling/classErrors/NoOverrideError.h"
// #include "../errorHandling/classErrors/VirtualNonMethod.h"
//
// // ---------- just how ----------
// #include "../errorHandling/how/HowDidYouGetHere.h"
//
// #include "files/FileGraph.h"
// #include "symbols/Type/ClassType.h"
//
//
// Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), len(tokens.size()), pos(0), hasMain(false), symTable(SymbolTable())
// {
// }
//
// Parser::~Parser() = default;
//
// std::vector<std::pair<std::filesystem::path, Token>> Parser::readIncludes()
// {
//     std::vector<std::pair<std::filesystem::path, Token>> v;
//     if (includes.empty() && pos == 0)
//     {
//         while (match(TokenType::KEYWORD, "feat"))
//         {
//             advance();
//
//             Token pathToken;
//             if (!expectAndGet(TokenType::CONST_STR, new ExpectedAPath(current()), pathToken)) return {};
//
//             std::string wstr = pathToken.value;
//             wstr.erase(
//                 std::ranges::remove(wstr, L'"').begin(),
//                 wstr.end()
//             );
//             std::filesystem::path path = wstr;
//             if (!match(TokenType::PUNCTUATION, "║"))
//             {
//                 addError(new MissingSemicolon(current()));
//                 return {};
//             }
//             advance(); // eat the semicolon
//
//             if (path.extension() != ".cb")
//             {
//                 addError(new InvalidPathExtension(current()));
//                 return {};
//             }
//
//             includes.emplace_back(std::make_unique<IncludeStmt>(pathToken, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), path));
//             v.emplace_back(path, pathToken);
//         }
//     }
//
//     return v;
// }
//
// void Parser::parse()
// {
//     auto block = parseBodyStmt({},true);
//     if (!block) return;
//     for (auto& stmt : block->getStmts())
//     {
//         stmts.push_back(std::move(stmt));
//     }
// }
//
// void Parser::analyze()
// {
//     while (!creditsQ.empty())
//     {
//         if (!symTable.isLegalCredit(creditsQ.front()))
//         {
//             addError(new IllegalCredit(creditsQ.front().getToken(), creditsQ.front().getName()));
//         }
//
//         creditsQ.pop();
//     }
//
//     while (!callsQ.empty())
//     {
//         FuncCallExpr* call = callsQ.front();
//         std::unique_ptr<IType> t = symTable.getCallType(call, call->getCurrClass());
//
//         if (t == nullptr)
//         {
//             addError(new IllegalCall(call->getToken(), call->getName()));
//         }
//
//         call->setType(std::move(t));
//
//         callsQ.pop();
//     }
//
//     for (const auto& stmt : stmts)
//     {
//         try {stmt->analyze();}
//         catch (Error& e) {addError(e.copy());}
//     }
// }
//
// std::string Parser::translateToCpp(const std::filesystem::path& hPath, const bool isMain) const
// {
//     std::ostringstream oss;
//
//     oss << "#include <iostream>" << std::endl;
//
//     if (isMain) oss << translateToH(isMain);
//     else oss << "#include \"" << hPath.generic_string() << "\"" << std::endl;
//
//     for (const auto& stmt : stmts)
//     {
//         oss << std::endl << stmt->translateToCpp();
//     }
//
//     if (isMain)
//     {
//         oss << "int main()" << std::endl <<
//             "{" << std::endl <<
//             "\treturn prelude().getValue();" << std::endl <<
//             "}" << std::endl;
//     }
//
//     return oss.str();
// }
//
// std::string Parser::translateToH(const bool isMain) const
// {
//     std::ostringstream oss;
//     if (!isMain) oss << "#pragma once" << std::endl;
//
//     oss << "#include <string>" << std::endl;
//     oss << Utils::getAllObjIncludes() << std::endl;
//
//     if (!includes.empty()) oss << std::endl;
//
//     for (const auto& i : includes)
//     {
//         oss << i->translateToCpp() << std::endl;
//     }
//
//     for (const auto& stmt : stmts) oss << stmt->translateToH();
//
//     return oss.str();
// }
//
// void Parser::addToSymTable(const SymbolTable& symTable)
// {
//     this->symTable += symTable;
// }
//
// const SymbolTable& Parser::getSymTable() const
// {
//     return symTable;
// }
//
// bool Parser::getHasMain() const
// {
//     return hasMain;
// }
//
// const Token& Parser::getLast() const
// {
//     return tokens[len - 1];
// }
//
// const std::vector<std::unique_ptr<Error>>& Parser::getErrors() const
// {
//     return errors;
// }
//
// bool Parser::shouldProduceCpp(bool isMain) const
// {
//     if (isMain || hasMain) return true;
//
//     for (const auto& stmt : stmts)
//     {
//         if (dynamic_cast<IncludeStmt*>(stmt.get())) continue;
//
//         if (auto classStmt = dynamic_cast<ClassDeclStmt*>(stmt.get()))
//         {
//             if (classStmt->getCurrClass() && !classStmt->getCurrClass()->isAbstract())
//             {
//                 return true; // Found a concrete class
//             }
//         }
//         else if (dynamic_cast<FuncDeclStmt*>(stmt.get()))
//         {
//             return true; // Found a global function implementation
//         }
//         else
//         {
//             // Any other statement in global scope (e.g. VarDeclStmt) needs a .cpp
//             return true;
//         }
//     }
//
//     // If it only has includes or abstract classes, don't produce a .cpp
//     return false;
// }
//
// void Parser::addError(Error* err)
// {
//     errors.emplace_back(err);
// }
//
// void Parser::synchronize()
// {
//     advance();
//
//     // Clear callsQ and creditsQ to prevent dangling pointers from incomplete statements/expressions
//     while (!callsQ.empty()) callsQ.pop();
//     while (!creditsQ.empty()) creditsQ.pop();
//
//     while (!isAtEnd())
//     {
//         if (prev().type == TokenType::PUNCTUATION && prev().value == "║") return;
//
//         switch (current().type)
//         {
//             case TokenType::KEYWORD:
//                 // Check for keywords that start a new statement
//                 if (current().value == "song" || current().value == "instrument" ||
//                     current().value == "D" || current().value == "G" ||
//                     current().value == "A" || current().value == "Fmaj" ||
//                     current().value == "Fmin" || current().value == "B" ||
//                     current().value == "feat" || current().value == "play" ||
//                     current().value == "playBar" || current().value == "hear" ||
//                     current().value == "pause" || current().value == "resume")
//                 {
//                     return;
//                 }
//             default:
//                 break;
//         }
//
//         advance();
//     }
// }
//
//
// const Token& Parser::current() const
// {
//     if (pos >= len) return tokens.back(); // Should not happen if isAtEnd is used properly
//     return tokens[pos];
// }
//
// Token Parser::advance()
// {
//     if (!isAtEnd())
//     {
//         pos++;
//     }
//     return prev();
// }
//
// const Token& Parser::peek() const
// {
//     if (pos + 1 >= len)
//     {
//         return tokens.back();
//     }
//     return tokens[pos + 1];
// }
//
// const Token& Parser::prev() const
// {
//     if (pos == 0) return tokens.front();
//     return tokens[pos - 1];
// }
//
// bool Parser::isAtEnd() const
// {
//     return pos >= len;
// }
//
// bool Parser::match(const TokenType type) const
// {
//     if (isAtEnd()) return false;
//     return current().type == type;
// }
//
// bool Parser::match(const TokenType type, const std::string& value) const
// {
//     if (isAtEnd()) return false;
//     const Token t = current();
//
//     return t.type == type && t.value == value;
// }
//
// bool Parser::expect(const TokenType type)
// {
//     if (!match(type))
//     {
//         addError(new UnexpectedToken(current()));
//         return false;
//     }
//     advance();
//     return true;
// }
//
// bool Parser::expect(const TokenType type, const std::string& value)
// {
//     if (!match(type, value))
//     {
//         addError(new UnexpectedToken(current()));
//         return false;
//     }
//     advance();
//     return true;
// }
//
// bool Parser::expect(const TokenType type, Error* err)
// {
//     if (!match(type))
//     {
//         addError(err);
//         return false;
//     }
//     advance();
//     delete err;
//     return true;
// }
//
// bool Parser::expect(const TokenType type, const std::string& value, Error* err)
// {
//     if (!match(type, value))
//     {
//         addError(err);
//         return false;
//     }
//     advance();
//     delete err;
//     return true;
// }
//
// bool Parser::expectAndGet(TokenType type, Token& out)
// {
//     if (!expect(type)) return false;
//     out = prev();
//     return true;
// }
//
// bool Parser::expectAndGet(TokenType type, const std::string& value, Token& out)
// {
//     if (!expect(type, value)) return false;
//     out = prev();
//     return true;
// }
//
// bool Parser::expectAndGet(TokenType type, Error* err, Token& out)
// {
//     if (!expect(type, err)) return false;
//     out = prev();
//     return true;
// }
//
// bool Parser::expectAndGet(TokenType type, const std::string& value, Error* err, Token& out)
// {
//     if (!expect(type, value, err)) return false;
//     out = prev();
//     return true;
// }
//
// bool Parser::isAssignmentStmtAhead()
// {
//     const size_t startPos = pos;
//
//     // Must start with identifier
//     if (!match(TokenType::IDENTIFIER))
//     {
//         return false;
//     }
//
//     advance(); // consume IDENTIFIER
//
//     while (true)
//     {
//         // Handle member access: \identifier
//         if (match(TokenType::PUNCTUATION, "\\"))
//         {
//             advance(); // consume '\'
//
//             if (!match(TokenType::IDENTIFIER))
//             {
//                 pos = startPos;
//                 return false;
//             }
//
//             advance(); // consume IDENTIFIER
//         }
//         // Handle indexing: [...]
//         else if (match(TokenType::PUNCTUATION, "["))
//         {
//             advance(); // consume '['
//
//             int depth = 1;
//             while (depth > 0)
//             {
//                 if (match(TokenType::PUNCTUATION, "["))
//                 {
//                     depth++;
//                 }
//                 else if (match(TokenType::PUNCTUATION, "]"))
//                 {
//                     depth--;
//                 }
//
//                 advance();
//             }
//         }
//         else
//         {
//             break;
//         }
//     }
//
//     const bool result =
//         match(TokenType::OP_ASSIGNMENT) ||
//         match(TokenType::PUNCTUATION, "║");
//
//     pos = startPos;
//     return result;
// }
//
// bool Parser::isUnaryOpStmtAhead()
// {
//     const size_t startPos = pos;
//
//     if (!match(TokenType::IDENTIFIER))
//     {
//         return false;
//     }
//
//     advance(); // consume IDENTIFIER
//
//     while (true)
//     {
//         // Handle member access: \identifier
//         if (match(TokenType::PUNCTUATION, "\\"))
//         {
//             advance(); // consume '\'
//
//             if (!match(TokenType::IDENTIFIER))
//             {
//                 pos = startPos;
//                 return false;
//             }
//
//             advance(); // consume IDENTIFIER
//         }
//         // Handle indexing: [...]
//         else if (match(TokenType::PUNCTUATION, "["))
//         {
//             advance(); // consume '['
//
//             int depth = 1;
//             while (depth > 0)
//             {
//                 if (match(TokenType::PUNCTUATION, "["))
//                 {
//                     depth++;
//                 }
//                 else if (match(TokenType::PUNCTUATION, "]"))
//                 {
//                     depth--;
//                 }
//
//                 advance();
//             }
//         }
//         else
//         {
//             break;
//         }
//     }
//
//     bool result = match(TokenType::OP_UNARY);
//
//     pos = startPos;
//     return result;
// }
//
// std::unique_ptr<VarDeclStmt> Parser::parseVarDecStmt(const bool isField)
// {
//     bool isStatic = false;
//     if (match(TokenType::KEYWORD, "unison"))
//     {
//         isStatic = true;
//         advance();
//     }
//
//     if (match(TokenType::TYPE, "riff"))
//     {
//         auto res = parseArrayDeclStmt();
//         if (res && isStatic) const_cast<Var&>(res->getVar()).setStatic(true);
//         return res;
//     }
//
//     const Token& t = current();
//     const std::unique_ptr<IType> varType = parseIType();
//     if (!varType) return nullptr;
//
//     Token identifierToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), identifierToken)) return nullptr;
//     const std::string varName = identifierToken.value;
//
//     const Var var(varType->copy(), varName, isStatic);
//
//     const bool startsMusical = Utils::startsWithNote(varName);
//
//     if (isField && !startsMusical)
//     {
//         addError(new IllegalFieldName(identifierToken));
//         return nullptr;
//     }
//
//     if (!isField && startsMusical)
//     {
//         addError(new IllegalVarName(identifierToken));
//         return nullptr;
//     }
//
//     if (isField && !match(TokenType::OP_ASSIGNMENT, "="))
//     {
//         symTable.addVar(varType->copy(), identifierToken);
//         return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), false, nullptr, var);
//     }
//
//     if (match(TokenType::PUNCTUATION, "║"))
//     {
//         symTable.addVar(varType->copy(), identifierToken);
//         advance();
//         return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), false, nullptr, var);
//     }
//
//     if (!expect(TokenType::OP_ASSIGNMENT, "=", new NoPlacementOperator(current()))) return nullptr;
//
//     auto expr = parseExpr();
//     if (!expr) return nullptr;
//
//     if (!isField)
//     {
//         if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//         symTable.addVar(varType->copy(), identifierToken);
//     }
//
//     return std::make_unique<VarDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), true, std::move(expr), var);
// }
//
// std::unique_ptr<AssignmentStmt> Parser::parseAssignmentStmt()
// {
//     const Token& t = current();
//     std::unique_ptr<Call> call = parseVarCallExpr(false);
//
//     if (!call) return nullptr;
//
//     Token opToken;
//     if (!expectAndGet(TokenType::OP_ASSIGNMENT, opToken)) return nullptr;
//     const std::string op = opToken.value;
//
//     auto expr = parseExpr();
//     if (!expr) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<AssignmentStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, std::move(expr));
// }
//
// std::unique_ptr<HearStmt> Parser::parseHearStmt()
// {
//     std::vector<std::unique_ptr<Call>> calls;
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "hear", new HowDidYouGetHere(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingBrace(current()))) return nullptr;
//
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         auto call = parseCallExpr();
//         if (!call) return nullptr;
//         calls.push_back(std::move(call));
//
//         if (match(TokenType::PUNCTUATION, ")"))
//         {
//             break;
//         }
//
//         if (!expect(TokenType::PUNCTUATION, ",", new InvalidExpression(current()))) return nullptr;
//     }
//     advance();
//
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<HearStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), calls);
// }
//
// std::unique_ptr<PlayStmt> Parser::parsePlayStmt()
// {
//     bool newline = false;
//     std::vector<std::unique_ptr<Expr>> args;
//     const Token& t = current();
//
//     if (match(TokenType::KEYWORD, "play"))
//     {
//         advance();
//     }
//     else if (match(TokenType::KEYWORD, "playBar"))
//     {
//         newline = true;
//         advance();
//     }
//     else
//     {
//         addError(new HowDidYouGetHere(current()));
//         return nullptr;
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingBrace(current()))) return nullptr;
//
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         auto arg = parseExpr();
//         if (!arg) return nullptr;
//         args.push_back(std::move(arg));
//
//         if (match(TokenType::PUNCTUATION, ")"))
//         {
//             break;
//         }
//
//         if (!expect(TokenType::PUNCTUATION, ",", new InvalidExpression(prev()))) return nullptr;
//     }
//     advance();
//
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<PlayStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(args), newline);
// }
//
// std::unique_ptr<BodyStmt> Parser::parseBodyStmt(const std::vector<std::pair<Var, const Token>>& args, const bool isGlobal, const bool isBreakable, const bool isContinueAble, const bool hasBrace)
// {
//     const Token& t = current();
//     if (!isGlobal && hasBrace)
//     {
//         if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return nullptr;
//         symTable.enterScope(isBreakable, isContinueAble);
//
//         if (!args.empty())
//         {
//             for (const auto& p : args)
//             {
//                 symTable.addVar(p.first, p.second);
//             }
//         }
//     }
//
//     std::vector<std::unique_ptr<Stmt>> bodyStmts;
//
//     while (!isAtEnd() && !match(TokenType::PUNCTUATION, "☉"))
//     {
//         std::unique_ptr<Stmt> stmt = nullptr;
//         if (match(TokenType::TYPE))
//         {
//             stmt = parseVarDecStmt();
//         }
//         else if (match(TokenType::IDENTIFIER) && peek().type == TokenType::IDENTIFIER && SymbolTable::isClass(current().value))
//         {
//             stmt = parseObjCreationStmt();
//             if (!stmt)
//             {
//                 synchronize();
//                 continue;
//             }
//         }
//         else if (match(TokenType::IDENTIFIER) && isAssignmentStmtAhead())
//         {
//             stmt = parseAssignmentStmt();
//         }
//         else if (match(TokenType::KEYWORD, "hear"))
//         {
//             stmt = parseHearStmt();
//         }
//         else if (match(TokenType::KEYWORD, "play") ||
//                  match(TokenType::KEYWORD, "playBar"))
//         {
//             stmt = parsePlayStmt();
//         }
//         else if (isUnaryOpStmtAhead())
//         {
//             stmt = parseUnaryOpExpr(true);
//         }
//         else if (match(TokenType::KEYWORD, "song"))
//         {
//             stmt = parseFuncDeclStmt();
//         }
//         else if (match(TokenType::KEYWORD, "B"))
//         {
//             stmt = parseReturnStmt();
//         }
//         else if (match(TokenType::IDENTIFIER) && peek().type == TokenType::PUNCTUATION && peek().value == "(")
//         {
//             stmt = parseFuncCallExpr(true);
//         }
//         else if (match(TokenType::IDENTIFIER_CALL))
//         {
//             stmt = parseConstractorCallStmt();
//             if (stmt)
//             {
//                 dynamic_cast<ConstractorCallStmt*>(stmt.get())->setIsStmt(true);
//                 if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current())))
//                 {
//                     stmt = nullptr;
//                 }
//             }
//         }
//         else if (match(TokenType::KEYWORD, "D"))
//         {
//             stmt = parseIfStmt();
//         }
//         else if (match(TokenType::KEYWORD, "G"))
//         {
//             stmt = parseWhileStmt();
//         }
//         else if (match(TokenType::KEYWORD, "A"))
//         {
//             stmt = parseSwitchStmt();
//         }
//         else if (match(TokenType::KEYWORD, "pause"))
//         {
//             if (!symTable.getCurrScope()->getIsBreakable())
//             {
//                 addError(new StmtNotBreakable(current()));
//                 synchronize();
//                 continue;
//             }
//             stmt = parseBreakStmt();
//             if (!hasBrace)
//             {
//                 break;
//             }
//         }
//         else if (match(TokenType::KEYWORD, "resume"))
//         {
//             if (!symTable.getCurrScope()->getIsContinueAble())
//             {
//                 addError(new StmtNotContinueAble(current()));
//                 synchronize();
//                 continue;
//             }
//             stmt = parseContinueStmt();
//             if (!hasBrace)
//             {
//                 break;
//             }
//         }
//         else if (match(TokenType::KEYWORD, "Fmaj") || match(TokenType::KEYWORD, "Fmin"))
//         {
//             stmt = parseForStmt();
//         }
//         else if (match(TokenType::KEYWORD, "instrument"))
//         {
//             stmt = parseClassDeclStmt();
//         }
//         else if (match(TokenType::COMMENT_MULTI) || match(TokenType::COMMENT_SINGLE))
//         {
//             advance();
//             continue;
//         }
//         else if (match(TokenType::IDENTIFIER) && peek().type == TokenType::PUNCTUATION && peek().value == "\\")
//         {
//             stmt = parseExpr(false, true);
//         }
//         else if (match(TokenType::KEYWORD, "feat"))
//         {
//             addError(new IncludeNotInTop(current()));
//             synchronize();
//             continue;
//         }
//         else if (match(TokenType::KEYWORD, "rest") || match(TokenType::KEYWORD, "motif") || match(TokenType::KEYWORD, "variation"))
//         {
//             addError(new VirtualNonMethod(current()));
//             synchronize();
//             continue;
//         }
//         else
//         {
//             addError(new UnrecognizedToken(current()));
//             synchronize();
//             continue;
//         }
//
//         if(stmt)
//         {
//             bodyStmts.push_back(std::move(stmt));
//         }
//         else
//         {
//             synchronize();
//         }
//     }
//
//     if (!isGlobal && hasBrace)
//     {
//         if (isAtEnd())
//         {
//             addError(new MissingBrace(prev()));
//         }
//
//         if (!match(TokenType::PUNCTUATION, "☉"))
//         {
//             addError(new MissingBrace(current()));
//         } else if (!isAtEnd())
//         {
//             advance();
//         }
//         symTable.exitScope();
//     }
//
//     return std::make_unique<BodyStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), bodyStmts, isGlobal);
// }
//
// std::unique_ptr<FuncDeclStmt> Parser::parseFuncDeclStmt(const bool isMethod)
// {
//     std::vector<std::pair<Var, const Token>> args;
//     std::vector<std::unique_ptr<FuncCreditStmt>> credited;
//     IFuncDeclStmt* currFunc = symTable.getCurrFunc();
//     const Token& t = current();
//     VirtualType vType = VirtualType::NONE;
//     bool isStatic = false;
//
//     if (match(TokenType::KEYWORD, "unison"))
//     {
//         isStatic = true;
//         advance();
//     }
//
//     if ((match(TokenType::KEYWORD, "motif") || match(TokenType::KEYWORD, "rest") || match(TokenType::KEYWORD, "variation")) && !isMethod)
//     {
//         addError(new VirtualNonMethod(current()));
//     }
//     if ((match(TokenType::KEYWORD, "motif") || match(TokenType::KEYWORD, "rest") || match(TokenType::KEYWORD, "variation")) && isStatic)
//     {
//         addError(new StaticFuncCantVirtual(current()));
//     }
//
//     if (match(TokenType::KEYWORD, "motif"))
//     {
//         vType = VirtualType::VIRTUAL;
//         advance();
//     }
//     else if (match(TokenType::KEYWORD, "rest"))
//     {
//         vType = VirtualType::PURE;
//         advance();
//     }
//
//     else if (match(TokenType::KEYWORD, "variation"))
//     {
//         vType = VirtualType::OVERRIDE;
//         advance();
//     }
//
//     if (!expect(TokenType::KEYWORD, "song", new ExpectedKeywordNotFound(current(), "song"))) return nullptr;
//
//     if (match(TokenType::PUNCTUATION, "©"))
//     {
//         advance();
//         if (!expect(TokenType::PUNCTUATION, "(", new MissingBrace(current()))) return nullptr;
//         while (!match(TokenType::PUNCTUATION, ")"))
//         {
//             auto credit = parseFuncCreditStmt();
//             if (!credit) return nullptr;
//             credited.push_back(std::move(credit));
//
//             if (!match(TokenType::PUNCTUATION, ")"))
//             {
//                 if (!expect(TokenType::PUNCTUATION, ",", new UnexpectedToken(current()))) return nullptr;
//             }
//         }
//         advance();
//     }
//
//     Token funcNameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), funcNameToken)) return nullptr;
//     const std::string funcName = funcNameToken.value;
//
//
//     if (funcName == "prelude" && hasMain)
//     {
//         addError(new MainOverride(current()));
//     }
//
//     const ClassNode* owner = isMethod ? symTable.getCurrClass() : nullptr;
//
//     if (isMethod)
//     {
//         if (owner->getClass().hasMethod(funcName))
//         {
//             addError(new IdentifierTaken(current()));
//         }
//
//         if (vType == VirtualType::OVERRIDE)
//         {
//             const ClassNode* parent = owner->getParent();
//             const Func* baseMethod = (parent != nullptr) ? parent->findMethod(funcName) : nullptr;
//
//             if (baseMethod == nullptr)
//             {
//                 addError(new NoOverrideError(current()));
//             }
//             else if (baseMethod->getVirtual() != VirtualType::VIRTUAL && baseMethod->getVirtual() != VirtualType::PURE)
//             {
//                 addError(new NoOverrideError(current()));
//             }
//         }
//     }
//     else if (symTable.doesFuncExist(funcName, nullptr))
//     {
//         addError(new IdentifierTaken(current()));
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingParenthesis(current()))) return nullptr;
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         const std::unique_ptr<IType> type = parseIType();
//
//         if (!type) return nullptr;
//
//         Token currNameToken;
//         if (!expectAndGet(TokenType::IDENTIFIER, new MissingBrace(current()), currNameToken)) return nullptr;
//         std::string currName = currNameToken.value;
//
//         args.emplace_back(Var(type->copy(), currName), current());
//
//         if (!match(TokenType::PUNCTUATION, ")"))
//         {
//             if (!expect(TokenType::PUNCTUATION, ",", new UnexpectedToken(current()))) return nullptr;
//         }
//     }
//     advance();
//
//     std::vector<Var> varArgs;
//
//     if (!args.empty())
//     {
//         for (const auto& key : args | std::views::keys)
//         {
//             varArgs.emplace_back(key.copy());
//         }
//     }
//
//     if (isMethod && vType == VirtualType::OVERRIDE)
//     {
//         const ClassNode* parent = owner->getParent();
//         if (parent != nullptr)
//         {
//             const Func* baseMethod = parent->findMethod(funcName);
//             if (baseMethod != nullptr)
//             {
//                 Func currentFunc(nullptr, funcName, varArgs, vType, owner);
//                 if (!baseMethod->isSameNameAndArgs(currentFunc))
//                 {
//                     addError(new InvalidOverrideSignature(current()));
//                 }
//             }
//         }
//     }
//
//
//     if (!match(TokenType::PUNCTUATION, "->"))
//     {
//         if (funcName == "prelude")
//         {
//             addError(new InvalidMainReturnType(current()));
//         }
//
//         auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), owner, funcName, std::make_unique<Type>("fermata"), varArgs, credited, isMethod, vType, isStatic);
//         funcDeclStmt->getFunc().setOwner(owner);
//
//         if (vType != VirtualType::PURE)
//         {
//             symTable.addFunc(funcDeclStmt->getFunc());
//             symTable.changeFunc(funcDeclStmt.get());
//             auto body = parseBodyStmt(args);
//             if (!body) return nullptr;
//             funcDeclStmt->setBody(std::move(body));
//             symTable.changeFunc(currFunc);
//         }
//         else
//         {
//              symTable.addFunc(funcDeclStmt->getFunc());
//         }
//         return std::move(funcDeclStmt);
//     }
//     advance();
//
//     const std::unique_ptr<IType> rType = parseIType();
//     if (!rType) return nullptr;
//
//     if (funcName == "prelude")
//     {
//         if (!args.empty())
//         {
//             addError(new InvalidMainArgs(current()));
//         }
//
//         if (rType->getType() != "degree")
//         {
//             addError(new InvalidMainReturnType(current()));
//         }
//
//         hasMain = true;
//     }
//
//     auto funcDeclStmt = std::make_unique<FuncDeclStmt>(t, symTable.getCurrScope(), owner, funcName, rType->copy(), varArgs, credited, isMethod, vType, isStatic);
//     funcDeclStmt->getFunc().setOwner(owner);
//
//     if (vType != VirtualType::PURE)
//     {
//         symTable.addFunc(funcDeclStmt->getFunc());
//         symTable.changeFunc(funcDeclStmt.get());
//         auto body = parseBodyStmt(args);
//         if (!body) return nullptr;
//         funcDeclStmt->setBody(std::move(body));
//
//         if (!funcDeclStmt->getHasReturned())
//         {
//             addError(new NoReturn(prev()));
//         }
//         symTable.changeFunc(currFunc);
//     }
//     else
//     {
//          symTable.addFunc(funcDeclStmt->getFunc());
//     }
//
//     return std::move(funcDeclStmt);
// }
//
// std::unique_ptr<ReturnStmt> Parser::parseReturnStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "B", new HowDidYouGetHere(current()))) return nullptr;
//
//     IFuncDeclStmt* currFunc = symTable.getCurrFunc();
//     std::unique_ptr<Expr> expr = nullptr;
//
//     if (currFunc->getReturnType()->getType() != "fermata")
//     {
//         if (!expect(TokenType::PUNCTUATION, "\\", new ExpectedAnExpression(current()))) return nullptr;
//         expr = parseExpr();
//         if (!expr) return nullptr;
//         if (*(currFunc->getReturnType()) != *(expr->getType()) && expr->getType()->getType() != "fermata")
//         {
//             addError(new WrongReturnType(current()));
//         }
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     currFunc->setHasReturned(true);
//
//     return std::make_unique<ReturnStmt>(t, symTable.getCurrScope(), currFunc,
//         symTable.getCurrClass(), expr, std::move(currFunc->getReturnType()->copy()));
// }
//
// std::unique_ptr<FuncCreditStmt> Parser::parseFuncCreditStmt()
// {
//     const Token& t = current();
//     Token creditToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), creditToken)) return nullptr;
//
//     FuncCredit credit(creditToken.value, current());
//
//     creditsQ.push(credit);
//
//     return std::make_unique<FuncCreditStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), credit);
// }
//
// std::unique_ptr<IfStmt> Parser::parseIfStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "D", new HowDidYouGetHere(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "|", new MissingPipe(current()))) return nullptr;
//     std::unique_ptr<Expr> expr = parseExpr();
//     if (!expr) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "|", new MissingPipe(current()))) return nullptr;
//     constexpr std::vector<std::pair<Var, const Token>> args;
//     std::unique_ptr<Stmt> body = parseBodyStmt(args, false);
//     if (!body) return nullptr;
//
//     if (match(TokenType::KEYWORD, "E"))
//     {
//         advance();
//         if (match(TokenType::PUNCTUATION, "\\"))
//         {
//             advance();
//             auto elseIf = parseIfStmt();
//             if (!elseIf) return nullptr;
//             return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), std::move(elseIf), true);
//         }
//         auto elseBody = parseBodyStmt(args, false);
//         if (!elseBody) return nullptr;
//         return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), std::move(elseBody), false);
//     }
//     return std::make_unique<IfStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(expr), std::move(body), nullptr, false);
// }
//
// std::unique_ptr<ArrayDeclStmt> Parser::parseArrayDeclStmt()
// {
//     const Token& t = current();
//     std::unique_ptr<IType> arrType = parseIType();
//     if (!arrType) return nullptr;
//
//     const unsigned int arrLevel = arrType->getArrLevel();
//     Token nameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
//     const std::string name = nameToken.value;
//
//     std::vector<std::unique_ptr<Expr>> sizes;
//     std::unique_ptr<Expr> startExpr = nullptr;
//
//     Var var(std::move(arrType), name);
//
//     symTable.addVar(var.copy(), current());
//
//     for (unsigned int i = 0; i < arrLevel; i++)
//     {
//         if (!expect(TokenType::PUNCTUATION, "[", new MissingBrace(current()))) return nullptr;
//
//         if (match(TokenType::PUNCTUATION, "]"))
//         {
//             advance();
//             sizes.push_back(
//                 std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>("degree"), "1")
//                 );
//         }
//         else
//         {
//             auto sizeExpr = parseExpr();
//             if (!sizeExpr) return nullptr;
//             sizes.push_back(std::move(sizeExpr));
//             if (!expect(TokenType::PUNCTUATION, "]", new MissingBrace(current()))) return nullptr;
//         }
//     }
//
//     if (match(TokenType::OP_ASSIGNMENT, "="))
//     {
//         advance();
//         startExpr = parseExpr();
//         if (!startExpr) return nullptr;
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//     return std::make_unique<ArrayDeclStmt>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         startExpr != nullptr,
//         std::move(startExpr),
//         var,
//         std::move(sizes)
//     );
// }
//
// std::unique_ptr<WhileStmt> Parser::parseWhileStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "G", new HowDidYouGetHere(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "║:", new MissingBrace(current()))) return nullptr;
//     std::unique_ptr<Expr> expr = parseExpr();
//     if (!expr) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, ":║", new MissingBrace(current()))) return nullptr;
//     constexpr std::vector<std::pair<Var, const Token>> args;
//     std::unique_ptr<Stmt> body = parseBodyStmt(args, false, true, true, true);
//     if (!body) return nullptr;
//
//     return std::make_unique<WhileStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), expr, body);
// }
//
// std::unique_ptr<BreakStmt> Parser::parseBreakStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "pause", new HowDidYouGetHere(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<BreakStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass());
// }
//
// std::unique_ptr<CaseStmt> Parser::parseCaseStmt()
// {
//     bool isDefault = false;
//     const Token& t = current();
//
//     if (!expect(TokenType::KEYWORD, "C")) return nullptr;
//     std::unique_ptr<Expr> e;
//     if (match(TokenType::PUNCTUATION, "\\"))
//     {
//         advance();
//         e = parseExpr();
//         if (!e) return nullptr;
//     }
//     else
//     {
//         isDefault = true;
//     }
//     if (!expect(TokenType::PUNCTUATION, "|", new MissingPipe(current()))) return nullptr;
//
//     symTable.enterScope(true, false);
//     std::vector<std::pair<Var, const Token>> args;
//     std::unique_ptr<BodyStmt> body = parseBodyStmt(args, false, true, false, false);
//     if (!body) return nullptr;
//     body->setHasBrace(false);
//
//     symTable.exitScope();
//
//     return std::make_unique<CaseStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(e), std::move(body), isDefault);
// }
//
// std::unique_ptr<SwitchStmt> Parser::parseSwitchStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "A", new HowDidYouGetHere(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "\\")) return nullptr;
//     const std::optional<Var> v = symTable.getCurrScope()->getVar(current().value, symTable.getCurrClass())->copy();
//
//     if (v == std::nullopt)
//     {
//         addError(new UnrecognizedIdentifier(current()));
//         return nullptr;
//     }
//
//     advance();
//     if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return nullptr;
//     std::vector<std::unique_ptr<CaseStmt>> cases;
//
//     while (!match(TokenType::PUNCTUATION, "☉"))
//     {
//         if (match(TokenType::KEYWORD, "C"))
//         {
//             auto caseStmt = parseCaseStmt();
//             if (!caseStmt) {
//                 synchronize();
//                 continue;
//             }
//             cases.push_back(std::move(caseStmt));
//         }
//         else
//         {
//             addError(new StmtInsideSwitchThatIsNotCase(current()));
//             synchronize();
//         }
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return nullptr;
//
//     return std::make_unique<SwitchStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), v.value().copy(), cases);
// }
//
// std::unique_ptr<ContinueStmt> Parser::parseContinueStmt()
// {
//     const Token& t = current();
//     if (!expect(TokenType::KEYWORD, "resume", new MissingBrace(current()))) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<ContinueStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass());
// }
//
// std::unique_ptr<ForStmt> Parser::parseForStmt()
// {
//     bool isIncreasing = false;
//     std::unique_ptr<Expr> startExpr = nullptr;
//     std::unique_ptr<Expr> stopExpr = nullptr;
//     std::unique_ptr<Expr> stepExpr = nullptr;
//     std::unique_ptr<BodyStmt> body = nullptr;
//     std::string varName = "i";
//     const Token& t = current();
//     Token varToken;
//
//     if (match(TokenType::KEYWORD, "Fmaj"))
//     {
//         isIncreasing = true;
//         advance();
//     }
//     else
//     {
//         if (!expect(TokenType::KEYWORD, "Fmin", new HowDidYouGetHere(current()))) return nullptr;
//     }
//
//     startExpr = parseExpr(false, false, false);
//     if (!startExpr) return nullptr;
//
//
//     if ((isIncreasing && match(TokenType::OP_UNARY, "♯"))|| (!isIncreasing && match(TokenType::OP_UNARY, "♭")))
//     {
//         advance();
//         stepExpr = parseExpr(false, false, false);
//         if (!stepExpr) return nullptr;
//     }
//
//     if (match(TokenType::PUNCTUATION, "#"))
//     {
//         advance();
//         stopExpr = parseExpr(false, false, false);
//         if (!stopExpr) return nullptr;
//     }
//
//     if (match(TokenType::PUNCTUATION, "\\"))
//     {
//         advance();
//         if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), varToken)) return nullptr;
//         varName = varToken.value;
//     } else {
//         varToken = Token(TokenType::IDENTIFIER, "i", current().line, current().column, current().path);
//     }
//
//     std::vector<std::pair<Var, const Token>> args;
//     args.emplace_back( Var(std::make_unique<Type>("degree"), varName), varToken);
//     body = parseBodyStmt(args, false, true, true, true);
//     if (!body) return nullptr;
//
//     return std::make_unique<ForStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(),
//         std::move(body), isIncreasing, std::move(startExpr), std::move(stepExpr), std::move(stopExpr), varName);
// }
//
// std::unique_ptr<ClassDeclStmt> Parser::parseClassDeclStmt()
// {
//     bool isInheriting = false;
//     std::string type;
//     std::string inheritingName = "";
//
//     Token t = current();
//     if (!expect(TokenType::KEYWORD, "instrument", new HowDidYouGetHere(t))) return nullptr;
//
//     Token nameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
//     const std::string name = nameToken.value;
//
//     if (match(TokenType::PUNCTUATION, "|"))
//     {
//         advance();
//         isInheriting = true;
//         if (match(TokenType::KEYWORD, "tutti"))
//         {
//             type = current().value;
//         }
//         else if (match(TokenType::KEYWORD, "sordino"))
//         {
//             type = current().value;
//         }
//         else if (match(TokenType::KEYWORD, "section"))
//         {
//             type = current().value;
//         }
//         else
//         {
//             addError(new InvalidAccessKeyword(current()));
//             return nullptr;
//         }
//         advance();
//         if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
//         if (SymbolTable::getClass(nameToken.value) == nullptr)
//         {
//             addError(new UnrecognizedIdentifier(current()));
//             return nullptr;
//         }
//         inheritingName = nameToken.value;
//     }
//
//     std::vector<Field> fields;
//     std::vector<Method> methods;
//     std::vector<Ctor> ctors;
//
//     ClassNode* parentPtr = nullptr;
//
//     if (!inheritingName.empty())
//     {
//         parentPtr = SymbolTable::getClass(inheritingName);
//     }
//
//     symTable.enterScope(false, false);
//     symTable.addClass(Class(name), parentPtr);
//
//     const size_t tempPos = pos;
//
//     if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return nullptr;
//     if (!parseFields(fields)) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return nullptr;
//
//     pos = tempPos;
//
//     if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return nullptr;
//     if (!parseMethods(methods)) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return nullptr;
//
//     pos = tempPos;
//
//     if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return nullptr;
//     if (!parseCtors(ctors)) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return nullptr;
//
//     auto declStmt = std::make_unique<ClassDeclStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, fields, methods, ctors, isInheriting, type, inheritingName);
//
//     if (!declStmt->getHasEmptyCtor())
//     {
//         std::vector<Var> emptyArgs;
//         symTable.addCtor(PUBLIC, Constractor(emptyArgs, name) ,t);
//     }
//
//     symTable.resetCurrClass();
//     symTable.exitScope();
//     return std::move(declStmt);
// }
//
// std::unique_ptr<ConstractorDeclStmt> Parser::parseCtor()
// {
//     const Token& t = current();
//     std::vector<std::pair<Var, const Token>> args;
//     std::vector<std::pair<Var, const Token>> emptyArgs;
//     std::vector<std::unique_ptr<FuncCreditStmt>> credited;
//     IFuncDeclStmt* currFunc = symTable.getCurrFunc();
//
//     if (!expect(TokenType::IDENTIFIER_CALL, new MissingIdentifier(t))) return nullptr;
//
//     const std::string funcName = t.value;
//
//     const ClassNode* pClass = symTable.getCurrClass();
//
//     if (pClass == nullptr) {
//         addError(new HowDidYouGetHere(t));
//         return nullptr;
//     }
//
//     // entering scope so we can do parent inits
//     symTable.enterScope(false, false);
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingParenthesis(current()))) return nullptr;
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         const std::unique_ptr<IType> type = parseIType();
//
//         if (!type) return nullptr;
//
//         Token currNameToken;
//         if (!expectAndGet(TokenType::IDENTIFIER, new MissingBrace(current()), currNameToken)) return nullptr;
//         std::string currName = currNameToken.value;
//
//         args.emplace_back(Var(type->copy(), currName), current());
//
//         if (!match(TokenType::PUNCTUATION, ")"))
//         {
//             if (!expect(TokenType::PUNCTUATION, ",", new UnexpectedToken(current()))) return nullptr;
//         }
//     }
//     advance();
//
//     std::vector<Var> varArgs;
//
//     if (!args.empty())
//     {
//         for (const auto& [var, varToken] : args )
//         {
//             varArgs.emplace_back(var.copy());
//             symTable.addVar(var.copy(), varToken);
//         }
//     }
//
//     if (pClass->getClass().hasConstractor(Constractor(varArgs, funcName)))
//     {
//         addError(new RedefOfCtor(t));
//     }
//
//     auto ctorDeclStmt = std::make_unique<ConstractorDeclStmt>(t, symTable.getCurrScope(), pClass, pClass->getClass().getClassName(), varArgs);
//     symTable.changeFunc(ctorDeclStmt.get());
//
//     if (match(TokenType::PUNCTUATION, "\\"))
//     {
//         advance();
//         Token token = current();
//         if (!expectAndGet(TokenType::KEYWORD, "bass", token) || !expectAndGet(TokenType::PUNCTUATION, "(", new MissingBrace(token), token)) return nullptr;
//
//         std::vector<std::unique_ptr<Expr>> parentArgs;
//
//         while (!match(TokenType::PUNCTUATION, ")"))
//         {
//             auto expr = parseExpr();
//
//             if (expr == nullptr) return nullptr;
//
//             parentArgs.emplace_back(std::move(expr));
//
//             if (!match(TokenType::PUNCTUATION, ")"))
//             {
//                 if (!expect(TokenType::PUNCTUATION, ",", new UnexpectedToken(current()))) return nullptr;
//             }
//         }
//
//         token = current();
//         if (!expectAndGet(TokenType::PUNCTUATION, ")", new MissingBrace(current()), token)) return nullptr;
//         ctorDeclStmt->setParentCtorCall(std::move(parentArgs));
//     }
//
//     auto body = parseBodyStmt(emptyArgs); // we already added the args
//     if (!body) return nullptr;
//     ctorDeclStmt->setBody(std::move(body));
//
//     symTable.exitScope();
//     symTable.changeFunc(currFunc);
//     return std::move(ctorDeclStmt);
// }
//
// std::unique_ptr<ConstractorCallStmt> Parser::parseConstractorCallStmt()
// {
//     const Token& t = current();
//     std::vector<std::unique_ptr<Expr>> args;
//
//     Token callToken;
//     if (!expectAndGet(TokenType::IDENTIFIER_CALL, new NoCtor(current()), callToken)) return nullptr;
//     const std::string call = callToken.value;
//     const std::string className = call.substr(0, call.length() - 5);
//     auto c = SymbolTable::getClass(className);
//
//     if (c == nullptr) {
//         addError(new ClassDosentExisit(t, className));
//         return nullptr;
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingParenthesis(current()))) return nullptr;
//
//     bool first = true;
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         auto arg = parseExpr();
//         if (!arg) return nullptr;
//         args.push_back(std::move(arg));
//
//         if (!first)
//         {
//             if (!expect(TokenType::PUNCTUATION, ",")) return nullptr;
//         }
//         first = false;
//     }
//     advance();
//     return std::make_unique<ConstractorCallStmt>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), c, std::move(args));
// }
//
// std::unique_ptr<ObjCreationStmt> Parser::parseObjCreationStmt()
// {
//     const Token& t = current();
//     Token classNameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(t), classNameToken)) return nullptr;
//     const std::string className = classNameToken.value;
//
//     auto c = SymbolTable::getClass(className);
//
//     if (c == nullptr) {
//         addError(new ClassDosentExisit(t, className));
//         return nullptr;
//     }
//
//     Token nameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(t), nameToken)) return nullptr;
//     const std::string name = nameToken.value;
//
//     const Var v = Var(std::make_unique<ClassType>(c),name);
//
//     if (match(TokenType::OP_ASSIGNMENT, "="))
//     {
//         advance();
//         auto ctorCall = parseConstractorCallStmt();
//         if (!ctorCall) return nullptr;
//         auto res = std::make_unique<ObjCreationStmt>(
//             t,
//             symTable.getCurrScope(),
//             symTable.getCurrFunc(),
//             symTable.getCurrClass(),
//             c,
//             true,
//             std::move(ctorCall),
//             v.copy()
//             );
//
//         symTable.addVar(v.copy(), t);
//         if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//         return std::move(res);
//     }
//
//     symTable.addVar(v.copy(), t);
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//     return std::make_unique<ObjCreationStmt>(
//             t,
//             symTable.getCurrScope(),
//             symTable.getCurrFunc(),
//             symTable.getCurrClass(),
//             c,
//             false,
//             nullptr,
//             v.copy()
//         );
// }
//
// bool Parser::parseCtorCall(const ClassNode* c)
// {
//     const Token& t = current();
//     std::vector<std::pair<Var, const Token>> args;
//
//     Token funcNameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER_CALL, new MissingIdentifier(t), funcNameToken)) return false;
//     const std::string funcName = funcNameToken.value;
//
//     if (c == nullptr) {
//         addError(new HowDidYouGetHere(t));
//         return false;
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingParenthesis(current()))) return false;
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         const std::unique_ptr<IType> type = parseIType();
//         if (!type) return false;
//
//         Token currNameToken;
//         if (!expectAndGet(TokenType::IDENTIFIER, new MissingBrace(current()), currNameToken)) return false;
//         std::string currName = currNameToken.value;
//
//
//         args.emplace_back(Var(type->copy(), currName), current());
//
//         if (!match(TokenType::PUNCTUATION, ")"))
//         {
//             if (!expect(TokenType::PUNCTUATION, ",", new UnexpectedToken(current()))) return false;
//         }
//     }
//     advance();
//
//     std::vector<Var> varArgs;
//
//     if (!args.empty())
//     {
//         for (const auto& key : args | std::views::keys)
//         {
//             varArgs.emplace_back(key.copy());
//         }
//     }
//
//     return c->getClass().hasConstractor(Constractor(varArgs, funcName));
// }
//
// bool Parser::parseFields(std::vector<Field>& fields)
// {
//     while (!isAtEnd() && !match(TokenType::PUNCTUATION, "☉"))
//     {
//         if (match(TokenType::KEYWORD, "playerScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::TYPE) || SymbolTable::getClass(current().value) || (match(TokenType::KEYWORD, "unison") && peek().type == TokenType::TYPE))
//                 {
//                     auto field = parseVarDecStmt(true);
//                     if (!field)
//                     {
//                         synchronize();
//                         continue;
//                     }
//
//                     if (!symTable.addField(PRIVATE, field->getVar().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//
//                     fields.emplace_back(PRIVATE, std::move(field));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "conductorScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::TYPE) || SymbolTable::getClass(current().value) || (match(TokenType::KEYWORD, "unison") && peek().type == TokenType::TYPE))
//                 {
//                     auto field = parseVarDecStmt(true);
//                     if (!field)
//                     {
//                         synchronize();
//                         continue;
//                     }
//
//                     if (!symTable.addField(PUBLIC, field->getVar().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//
//                     fields.emplace_back(PUBLIC, std::move(field));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "sectionScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::TYPE) || SymbolTable::getClass(current().value) || (match(TokenType::KEYWORD, "unison") && peek().type == TokenType::TYPE))
//                 {
//                     auto field = parseVarDecStmt(true);
//                     if (!field)
//                     {
//                         synchronize();
//                         continue;
//                     }
//
//                     if (!symTable.addField(PROTECTED, field->getVar().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//
//                     fields.emplace_back(PROTECTED, std::move(field));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else
//         {
//             addError(new UnrecognizedIdentifier(current()));
//             synchronize();
//         }
//     }
//
//     if (isAtEnd())
//     {
//         addError(new UnexpectedEOF(tokens[len-1]));
//         return false;
//     }
//     return true;
// }
//
// bool Parser::parseMethods(std::vector<Method>& methods)
// {
//     while (!isAtEnd() && !match(TokenType::PUNCTUATION, "☉"))
//     {
//         if (match(TokenType::KEYWORD, "playerScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::KEYWORD, "song") ||
//                     (match(TokenType::KEYWORD, "variation") ||
//                     match(TokenType::KEYWORD, "rest") ||
//                     match(TokenType::KEYWORD, "motif") ||
//                     match(TokenType::KEYWORD, "unison")) && (!isAtEnd() && peek().value == "song"))
//
//                 {
//                     auto method = parseFuncDeclStmt(true);
//                     if (!method)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addMethod(PRIVATE, method->getFunc().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     methods.emplace_back(PRIVATE, std::move(method));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "conductorScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::KEYWORD, "song") ||
//                     (match(TokenType::KEYWORD, "variation") ||
//                     match(TokenType::KEYWORD, "rest") ||
//                     match(TokenType::KEYWORD, "motif") ||
//                     match(TokenType::KEYWORD, "unison")) && (!isAtEnd() && peek().value == "song"))
//                 {
//                     auto method = parseFuncDeclStmt(true);
//                     if (!method)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addMethod(PUBLIC, method->getFunc().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     methods.emplace_back(PUBLIC, std::move(method));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "sectionScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::KEYWORD, "song") ||
//                     (match(TokenType::KEYWORD, "variation") ||
//                     match(TokenType::KEYWORD, "rest") ||
//                     match(TokenType::KEYWORD, "motif") ||
//                     match(TokenType::KEYWORD, "unison")) && (!isAtEnd() && peek().value == "song"))
//                 {
//                     auto method = parseFuncDeclStmt(true);
//                     if (!method)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addMethod(PROTECTED, method->getFunc().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     methods.emplace_back(PROTECTED, std::move(method));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else
//         {
//             addError(new UnrecognizedIdentifier(current()));
//             synchronize();
//         }
//     }
//
//     if (isAtEnd())
//     {
//         addError(new UnexpectedEOF(tokens[len-1]));
//         return false;
//     }
//     return true;
// }
//
// bool Parser::parseCtors(std::vector<Ctor>& ctors)
// {
//     while (!isAtEnd() && !match(TokenType::PUNCTUATION, "☉"))
//     {
//         if (match(TokenType::KEYWORD, "playerScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::IDENTIFIER_CALL))
//                 {
//                     auto ctor = parseCtor();
//                     if (!ctor)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addCtor(PRIVATE, ctor->getConstractor().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     ctors.emplace_back(PRIVATE, std::move(ctor));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "conductorScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::IDENTIFIER_CALL))
//                 {
//                     auto ctor = parseCtor();
//                     if (!ctor)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addCtor(PUBLIC, ctor->getConstractor().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     ctors.emplace_back(PUBLIC, std::move(ctor));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else if (match(TokenType::KEYWORD, "sectionScore"))
//         {
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "∮", new MissingBrace(current()))) return false;
//
//             while (true)
//             {
//                 if (match(TokenType::IDENTIFIER_CALL))
//                 {
//                     auto ctor = parseCtor();
//                     if (!ctor)
//                     {
//                         synchronize();
//                         continue;
//                     }
//                     if (!symTable.addCtor(PROTECTED, ctor->getConstractor().copy(), current()))
//                     {
//                         addError(new HowDidYouGetHere(current()));
//                     }
//                     ctors.emplace_back(PROTECTED, std::move(ctor));
//                 }
//                 else
//                 {
//                     unsigned int level = 0;
//                     while (true)
//                     {
//                         if (isAtEnd() || (level == 0 && (match(TokenType::PUNCTUATION, "|") || match(TokenType::PUNCTUATION, "║")))) break;
//
//                         if (match(TokenType::PUNCTUATION, "∮")) level++;
//                         else if (match(TokenType::PUNCTUATION, "☉")) level--;
//                         advance();
//                     }
//                 }
//
//                 if (match(TokenType::PUNCTUATION, "║"))
//                 {
//                     break;
//                 }
//
//                 if (!expect(TokenType::PUNCTUATION, "|", new MissingClassPipe(current()))) return false;
//             }
//             advance();
//             if (!expect(TokenType::PUNCTUATION, "☉", new MissingBrace(current()))) return false;
//         }
//         else
//         {
//             addError(new UnrecognizedIdentifier(current()));
//             synchronize();
//         }
//     }
//
//     if (isAtEnd())
//     {
//         addError(new UnexpectedEOF(tokens[len-1]));
//         return false;
//     }
//     return true;
// }
//
// std::unique_ptr<Call> Parser::parseFuncCallExpr(const bool isStmt)
// {
//     const Token& t = current();
//     Token nameToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), nameToken)) return nullptr;
//     const std::string name = nameToken.value;
//
//     std::vector<std::unique_ptr<Expr>> args;
//
//     if (!expect(TokenType::PUNCTUATION, "(", new MissingParenthesis(current()))) return nullptr;
//
//     bool first = true;
//     while (!match(TokenType::PUNCTUATION, ")"))
//     {
//         if (!first)
//         {
//             if (!expect(TokenType::PUNCTUATION, ",")) return nullptr;
//         }
//         first = false;
//         auto arg = parseExpr();
//         if (!arg) return nullptr;
//         args.push_back(std::move(arg));
//     }
//     advance();
//
//     if (isStmt)
//     {
//         if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//     }
//
//     auto expr = std::make_unique<FuncCallExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), name, std::move(args), isStmt);
//
//     callsQ.push(expr.get());
//
//     return expr;
// }
//
// std::unique_ptr<Call> Parser::parseCallExpr(const ClassNode* classCall)
// {
//     const Token& t = current();
//     Token varToken;
//     if (!expectAndGet(TokenType::IDENTIFIER, new MissingIdentifier(current()), varToken)) return nullptr;
//
//     std::optional<Var> var = symTable.getVar(varToken.value);
//
//     if (classCall != nullptr && !var.has_value())
//     {
//         if (const auto v = classCall->findField(varToken.value, classCall, true))
//         {
//             var.emplace(v->copy());
//         }
//     }
//
//     if (!var.has_value())
//     {
//         addError(new UnrecognizedIdentifier(prev()));
//         return nullptr;
//     }
//
//     std::unique_ptr<Call> call = std::make_unique<VarCallExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), var.value().copy());
//
//     if (match(TokenType::PUNCTUATION, "["))
//     {
//         return parseArrayAccess(std::move(call));
//     }
//
//     return call;
// }
//
// std::unique_ptr<Call> Parser::parseArrayAccess(std::unique_ptr<Call> call)
// {
//     while (match(TokenType::PUNCTUATION, "["))
//     {
//         const size_t savedPos = pos;
//
//         bool isSlicing = false;
//         size_t lookahead = pos + 1;
//         while (lookahead < tokens.size() && !(tokens[lookahead].type == TokenType::PUNCTUATION && tokens[lookahead].value == "]"))
//         {
//             if (tokens[lookahead].type == TokenType::PUNCTUATION && tokens[lookahead].value == ":")
//             {
//                 isSlicing = true;
//                 break;
//             }
//             ++lookahead;
//         }
//
//         pos = savedPos;
//         if (isSlicing)
//         {
//             call = parseArraySlicingExpr(std::move(call));
//         }
//         else
//         {
//             call = parseArrayIndexingExpr(std::move(call));
//         }
//         if (!call) return nullptr;
//     }
//
//     return call;
// }
//
// std::unique_ptr<Call> Parser::parseArraySlicingExpr(std::unique_ptr<Call> call)
// {
//     const Token& t = current();
//
//     std::unique_ptr<Expr> start = std::make_unique<ConstValueExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         std::make_unique<Type>("degree"),
//         "0"
//     );
//     std::unique_ptr<Expr> stop = std::make_unique<ConstValueExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         std::make_unique<Type>("degree"),
//         "-1"
//     );
//     std::unique_ptr<Expr> step = std::make_unique<ConstValueExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         std::make_unique<Type>("degree"),
//         "1"
//     );
//
//     if (!expect(TokenType::PUNCTUATION, "[", new MissingBrace(current()))) return nullptr;
//
//     if (!match(TokenType::PUNCTUATION, ":"))
//     {
//         start = parseExpr();
//         if (!start) return nullptr;
//     }
//
//     if (match(TokenType::PUNCTUATION, ":"))
//     {
//         advance();
//
//         if (!match(TokenType::PUNCTUATION, ":") && !match(TokenType::PUNCTUATION, "]"))
//         {
//             stop = parseExpr();
//             if (!stop) return nullptr;
//         }
//
//
//         if (match(TokenType::PUNCTUATION, ":"))
//         {
//             advance();
//
//             if (!match(TokenType::PUNCTUATION, "]"))
//             {
//                 step = parseExpr();
//                 if (!step) return nullptr;
//             }
//         }
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "]", new MissingBrace(current()))) return nullptr;
//
//     return std::make_unique<ArraySlicingExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         std::move(call),
//         std::move(start),
//         std::move(stop),
//         std::move(step)
//     );
// }
//
// std::unique_ptr<Call> Parser::parseArrayIndexingExpr(std::unique_ptr<Call> call)
// {
//     const Token& t = current();
//     if (!expect(TokenType::PUNCTUATION, "[", new MissingBrace(current()))) return nullptr;
//     std::unique_ptr<Expr> index = parseExpr();
//     if (!index) return nullptr;
//     if (!expect(TokenType::PUNCTUATION, "]", new MissingBrace(current()))) return nullptr;
//
//     return std::make_unique<ArrayIndexingExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(),
//         symTable.getCurrClass(),
//         std::move(call),
//         std::move(index)
//     );
// }
//
// std::unique_ptr<IType> Parser::parseIType()
// {
//     if (match(TokenType::TYPE, "riff"))
//     {
//         advance();
//         return parseArrayType();
//     }
//     if (match(TokenType::TYPE)) return parseType();
//     return parseClassType();
// }
//
// std::unique_ptr<Type> Parser::parseType()
// {
//     Token typeToken;
//     if (!expectAndGet(TokenType::TYPE, new InvalidNumberLiteral(current()), typeToken)) return nullptr;
//     std::string value = typeToken.value;
//
//     if (value == "sharp freq" || value == "flat freq")
//     {
//         addError(new UnrecognizedToken(typeToken));
//         return nullptr;
//     }
//
//     return std::make_unique<Type>(value);
// }
//
// std::unique_ptr<ArrayType> Parser::parseArrayType()
// {
//     std::unique_ptr<IType> arrType = parseIType();
//     if (!arrType) return nullptr;
//
//     return std::make_unique<ArrayType>(std::move(arrType));
// }
//
// std::unique_ptr<ClassType> Parser::parseClassType()
// {
//     const std::string classname = current().value;
//     if (!expect(TokenType::IDENTIFIER, new MissingBrace(current()))) return nullptr;
//
//     if (auto cls = SymbolTable::getClass(classname))
//     {
//         return std::make_unique<ClassType>(cls);
//     }
//
//     return nullptr;
// }
//
//
// std::unique_ptr<Expr> Parser::parseExpr(const bool hasParens, const bool isStmt, const bool allowBackslash)
// {
//     if (match(TokenType::IDENTIFIER))
//     {
//         if (peek().type == TokenType::OP_UNARY) return parseUnaryOpExpr(isStmt);
//         if (SymbolTable::getClass(current().value) != nullptr && peek().value == "\\") return parseStaticDotOpExpr(isStmt);
//     }
//
//     auto left = parsePrimary(isStmt, allowBackslash);
//     if (!left) return nullptr;
//
//     auto expr = parseBinaryOpRight(0, std::move(left), isStmt, allowBackslash, SymbolTable::getClass(left->getType()->getType()));
//     if (!expr) return nullptr;
//
//     if (hasParens)
//     {
//         expr->setHasParens(true);
//     }
//
//     return expr;
// }
//
// std::unique_ptr<Expr> Parser::parsePrimary(const bool isStmt, const bool allowBackslash, const ClassNode* classCall)
// {
//     const Token& t = current();
//
//     if (t.value == "+" || t.value == "-" ||
//          t.value == "*" || t.value == "/" ||
//          t.value == "//")
//     {
//         std::string op = t.value;
//         advance();
//
//         auto right = parseExpr(false, false, allowBackslash);
//         if (!right) return nullptr;
//
//         return std::make_unique<BinaryOpExpr>(
//             t,
//             symTable.getCurrScope(),
//             symTable.getCurrFunc(),
//             symTable.getCurrClass(),
//             op,
//             nullptr,
//             std::move(right)
//         );
//     }
//
//     if (current().isConst())
//     {
//         return parseConstValueExpr();
//     }
//
//
//     if (match(TokenType::IDENTIFIER))
//     {
//         if (peek().type == TokenType::PUNCTUATION && peek().value == "(")
//         {
//             return parseFuncCallExpr(isStmt);
//         }
//         return parseCallExpr(classCall);
//     }
//
//
//     if (match(TokenType::PUNCTUATION, "("))
//     {
//         advance();
//         auto expr = parseExpr(true, false, allowBackslash);
//         if (!expr) return nullptr;
//         if (!expect(TokenType::PUNCTUATION, ")", new MissingParenthesis(current()))) return nullptr;
//         return expr;
//     }
//
//     addError(new UnexpectedToken(current()));
//     return nullptr;
// }
//
// std::unique_ptr<Expr> Parser::parseBinaryOpRight(int exprPrec, std::unique_ptr<Expr> left, const bool isStmt, const bool allowBackslash, const ClassNode* classCall)
// {
//     while (true)
//     {
//         if (!(match(TokenType::OP_BINARY) || (allowBackslash && match(TokenType::PUNCTUATION, "\\"))))
//             return left;
//
//         const Token& t = current();
//         std::string op = t.value;
//         int prec = BinaryOpExpr::getPrecedence(op);
//
//         if (prec < exprPrec)
//             return left;
//
//         advance();
//
//         auto right = parsePrimary(isStmt, allowBackslash, classCall);
//         if (!right) return nullptr;
//
//         if (match(TokenType::OP_BINARY) || (allowBackslash && match(TokenType::PUNCTUATION, "\\")))
//         {
//             int nextPrec = BinaryOpExpr::getPrecedence(current().value);
//             if (nextPrec > prec)
//             {
//                 right->setIsStmt(false);
//                 right = parseBinaryOpRight(prec + 1, std::move(right), isStmt, allowBackslash);
//                 if (!right) return nullptr;
//             }
//         }
//
//         if (op == "\\")
//         {
//             auto callLeft = std::unique_ptr<Call>(
//                 dynamic_cast<Call*>(left.release())
//             );
//
//             auto callRight = std::unique_ptr<Call>(
//                 dynamic_cast<Call*>(right.release())
//             );
//
//             if (callLeft == nullptr || callRight == nullptr) {
//                 addError(new HowDidYouGetHere(current()));
//                 return nullptr;
//             }
//
//             callRight->setIsClassItem(true);
//
//             left = std::make_unique<DotOpExpr>(
//                 t,
//                 symTable.getCurrScope(),
//                 symTable.getCurrFunc(),
//                 symTable.getCurrClass(),
//                 std::move(callLeft),
//                 std::move(callRight)
//             );
//         }
//         else
//         {
//             left = std::make_unique<BinaryOpExpr>(
//                 t,
//                 symTable.getCurrScope(),
//                 symTable.getCurrFunc(),
//                 symTable.getCurrClass(),
//                 op,
//                 std::move(left),
//                 std::move(right)
//             );
//         }
//     }
// }
//
// std::unique_ptr<StaticDotOpExpr> Parser::parseStaticDotOpExpr(const bool isStmt)
// {
//     const Token& t = current();
//     if (!match(TokenType::IDENTIFIER))
//     {
//         addError(new HowDidYouGetHere(t));
//         return nullptr;
//     }
//     advance();
//
//     auto type = SymbolTable::getClass(t.value);
//
//     if (type == nullptr)
//     {
//         addError(new UnrecognizedIdentifier(t));
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "\\", new UnexpectedToken(current()))) return nullptr;
//
//     std::unique_ptr<Call> right = nullptr;
//
//     if (peek().value == "(") right = parseFuncCallExpr(isStmt);
//     else right = parseCallExpr(type);
//
//     if (!right) return nullptr;
//
//     return std::make_unique<StaticDotOpExpr>(
//         t,
//         symTable.getCurrScope(),
//         symTable.getCurrFunc(), symTable.getCurrClass(),
//         std::make_unique<ClassType>(type),
//         std::move(right),
//         isStmt);
// }
//
//
// std::unique_ptr<ConstValueExpr> Parser::parseConstValueExpr()
// {
//     const Token& t = current();
//     const TokenType tokenType = t.type;
//     std::string type;
//
//     switch (tokenType)
//     {
//     case TokenType::CONST_BOOL:
//         type = "mute";
//         break;
//     case TokenType::CONST_STR:
//         type = "bar";
//         break;
//     case TokenType::CONST_CHAR:
//         type = "note";
//         break;
//     case TokenType::CONST_FLOAT:
//         type = "freq";
//         break;
//     case TokenType::CONST_INT:
//         type = "degree";
//         break;
//     default:
//         addError(new InvalidExpression(t));
//         return nullptr;
//     }
//
//     advance();
//     return std::make_unique<ConstValueExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::make_unique<Type>(type), t.value);
// }
//
// std::unique_ptr<UnaryOpExpr> Parser::parseUnaryOpExpr(const bool isStmt)
// {
//     const Token& t = current();
//     std::unique_ptr<Call> call = parseVarCallExpr(false);
//
//     if (!call) return nullptr;
//     UnaryOp op;
//
//     Token opToken;
//     if (!expectAndGet(TokenType::OP_UNARY, opToken)) return nullptr;
//     const std::string value = opToken.value;
//
//
//     if (value == "♮")
//     {
//         op = UnaryOp::Zero;
//     }
//     else if (value == "♯")
//     {
//         op = UnaryOp::PlusPlus;
//     }
//     else if (value == "♭")
//     {
//         op = UnaryOp::MinusMinus;
//     }
//     else if (value == "!")
//     {
//         op = UnaryOp::Not;
//     }
//     else
//     {
//         addError(new UnexpectedToken(current()));
//         return nullptr;
//     }
//
//     if (!expect(TokenType::PUNCTUATION, "║", new MissingSemicolon(current()))) return nullptr;
//
//     return std::make_unique<UnaryOpExpr>(t, symTable.getCurrScope(), symTable.getCurrFunc(), symTable.getCurrClass(), std::move(call), op, isStmt);
// }
//
// std::unique_ptr<Call> Parser::parseVarCallExpr(const bool isStmt)
// {
//     const Token& t = current();
//
//     if (auto cls = SymbolTable::getClass(t.value))
//     {
//         return parseStaticDotOpExpr(false);
//     }
//
//     if(peek().value == "\\")
//     {
//         auto left = parseCallExpr();
//         const auto leftTypeWstr = left->getType()->getType();
//         auto expr = parseBinaryOpRight(0, std::move(left), false, true, SymbolTable::getClass(leftTypeWstr));
//
//         if (const auto callTemp = dynamic_cast<Call*>(expr.release()))
//         {
//             return std::unique_ptr<Call>(callTemp);
//         }
//
//         addError(new HowDidYouGetHere(t));
//         return nullptr;
//     }
//
//     return parseCallExpr();
// }
