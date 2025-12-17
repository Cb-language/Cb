cmake_minimum_required(VERSION 4.0)

# In Windows:
# Follow step 1: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell#1---set-up-vcpkg
# INSTALL DIRECTLY TO C:/.
if (WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows/share")
endif()

project(Transposer CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(Transposer)

add_executable(Transposer
        Transposer/files/FileManager.cpp
        Transposer/files/FileManager.h
        Transposer/Source.cpp
        Transposer/token/Token.cpp
        Transposer/token/Token.h
        Transposer/token/Tokenizer.cpp
        Transposer/token/Tokenizer.h
        Transposer/AST/abstract/Statement.h
        Transposer/errorHandling/ErrorHandler.cpp
        Transposer/errorHandling/ErrorHandler.h
        Transposer/errorHandling/lexicalErrors/UnidentifiedToken.cpp
        Transposer/errorHandling/lexicalErrors/UnidentifiedToken.h
        Transposer/errorHandling/Error.cpp
        Transposer/errorHandling/Error.h
        Transposer/errorHandling/lexicalErrors/InvalidNumberLiteral.cpp
        Transposer/errorHandling/lexicalErrors/InvalidNumberLiteral.h
        Transposer/errorHandling/lexicalErrors/UnterminatedString.cpp
        Transposer/errorHandling/lexicalErrors/UnterminatedString.h
        Transposer/errorHandling/lexicalErrors/InvalidEscapeSequence.cpp
        Transposer/errorHandling/lexicalErrors/InvalidEscapeSequence.h
        Transposer/errorHandling/lexicalErrors/UnexpectedEOF.cpp
        Transposer/errorHandling/lexicalErrors/UnexpectedEOF.h
        Transposer/errorHandling/lexicalErrors/InvalidCharacterEncoding.cpp
        Transposer/errorHandling/lexicalErrors/InvalidCharacterEncoding.h
        Transposer/errorHandling/syntaxErrors/MissingSemicolon.cpp
        Transposer/errorHandling/syntaxErrors/MissingSemicolon.h
        Transposer/errorHandling/syntaxErrors/MissingParenthesis.cpp
        Transposer/errorHandling/syntaxErrors/MissingParenthesis.h
        Transposer/errorHandling/syntaxErrors/MissingBrace.cpp
        Transposer/errorHandling/syntaxErrors/MissingBrace.h
        Transposer/errorHandling/syntaxErrors/UnexpectedToken.cpp
        Transposer/errorHandling/syntaxErrors/UnexpectedToken.h
        Transposer/errorHandling/syntaxErrors/MisplacedKeyword.cpp
        Transposer/errorHandling/syntaxErrors/MisplacedKeyword.h
        Transposer/errorHandling/syntaxErrors/InvalidExpression.cpp
        Transposer/errorHandling/syntaxErrors/InvalidExpression.h
        Transposer/errorHandling/syntaxErrors/InvalidStatement.cpp
        Transposer/errorHandling/syntaxErrors/InvalidStatement.h
        Transposer/errorHandling/syntaxErrors/MissingIdentifier.cpp
        Transposer/errorHandling/syntaxErrors/MissingIdentifier.h
        Transposer/parser/parser.cpp
        Transposer/parser/parser.h
        Transposer/symbols/Type/Type.cpp
        Transposer/symbols/Type/Type.h
        Transposer/AST/abstract/Expression.h
        Transposer/AST/statements/expression/ConstValueExpr.cpp
        Transposer/AST/statements/expression/ConstValueExpr.h
        Transposer/other/SymbolTable.cpp
        Transposer/other/SymbolTable.h
        Transposer/other/Scope.cpp
        Transposer/other/Scope.h
        Transposer/symbols/Var.cpp
        Transposer/symbols/Var.h
        Transposer/errorHandling/lexicalErrors/IdentifierTaken.cpp
        Transposer/errorHandling/lexicalErrors/IdentifierTaken.h
        Transposer/other/Utils.cpp
        Transposer/other/Utils.h
        Transposer/AST/statements/VarDeclStmt.cpp
        Transposer/AST/statements/VarDeclStmt.h
        Transposer/AST/statements/AssignmentStmt.cpp
        Transposer/AST/statements/AssignmentStmt.h
        Transposer/errorHandling/lexicalErrors/InvalidIdentifier.cpp
        Transposer/errorHandling/lexicalErrors/InvalidIdentifier.h
        Transposer/AST/statements/expression/VarCallExpr.cpp
        Transposer/AST/statements/expression/VarCallExpr.h
        Transposer/AST/statements/expression/BinaryOpExpr.cpp
        Transposer/AST/statements/expression/BinaryOpExpr.h
        Transposer/AST/statements/HearStmt.cpp
        Transposer/AST/statements/HearStmt.h
        Transposer/AST/statements/expression/UnaryOpExpr.cpp
        Transposer/AST/statements/expression/UnaryOpExpr.h
        Transposer/AST/statements/PlayStmt.cpp
        Transposer/AST/statements/PlayStmt.h
        Transposer/symbols/Func.cpp
        Transposer/symbols/Func.h
        Transposer/AST/statements/BodyStmt.cpp
        Transposer/AST/statements/BodyStmt.h
        Transposer/AST/statements/FuncDeclStmt.cpp
        Transposer/AST/statements/FuncDeclStmt.h
        Transposer/AST/statements/ReturnStmt.cpp
        Transposer/AST/statements/ReturnStmt.h
        Transposer/errorHandling/syntaxErrors/WrongReturnType.cpp
        Transposer/errorHandling/syntaxErrors/WrongReturnType.h
        Transposer/errorHandling/syntaxErrors/NoReturnStmt.cpp
        Transposer/errorHandling/syntaxErrors/NoReturnStmt.h
        Transposer/errorHandling/lexicalErrors/MainOverride.cpp
        Transposer/errorHandling/lexicalErrors/MainOverride.h
        Transposer/errorHandling/lexicalErrors/NoMain.cpp
        Transposer/errorHandling/lexicalErrors/NoMain.h
        Transposer/errorHandling/lexicalErrors/InvalidMainArgs.cpp
        Transposer/errorHandling/lexicalErrors/InvalidMainArgs.h
        Transposer/errorHandling/lexicalErrors/InvalidMainReturnType.cpp
        Transposer/errorHandling/lexicalErrors/InvalidMainReturnType.h
        Transposer/symbols/FuncCredit.cpp
        Transposer/symbols/FuncCredit.h
        Transposer/AST/statements/FuncCreditStmt.cpp
        Transposer/AST/statements/FuncCreditStmt.h
        Transposer/AST/statements/expression/FuncCallExpr.cpp
        Transposer/AST/statements/expression/FuncCallExpr.h
        Transposer/files/ArrayHelper.cpp
        Transposer/files/ArrayHelper.h
        Transposer/symbols/Type/IType.h
        Transposer/AST/statements/IfStmt.cpp
        Transposer/AST/statements/IfStmt.h
        Transposer/errorHandling/syntaxErrors/ElseWithoutIf.cpp
        Transposer/errorHandling/syntaxErrors/ElseWithoutIf.h
        Transposer/AST/statements/ArrayDeclStmt.cpp
        Transposer/AST/statements/ArrayDeclStmt.h
        Transposer/symbols/Type/ArrayType.cpp
        Transposer/symbols/Type/ArrayType.h
        Transposer/AST/statements/WhileStmt.cpp
        Transposer/AST/statements/WhileStmt.h
)

# In order to call the exe from the cmd
target_link_options(Transposer PRIVATE -static -static-libgcc -static-libstdc++)

include(FetchContent)


# Boost

find_package(Boost REQUIRED regex)
target_link_libraries(Transposer PRIVATE Boost::boost)