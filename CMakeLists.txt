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
        Transposer/errorHandling/Error.cpp
        Transposer/errorHandling/Error.h
        Transposer/errorHandling/errorTypes/UnexpectedEOF.cpp
        Transposer/errorHandling/errorTypes/UnexpectedEOF.h
        Transposer/errorHandling/errorTypes/MissingSemicolon.cpp
        Transposer/errorHandling/errorTypes/MissingSemicolon.h
        Transposer/errorHandling/errorTypes/MissingParenthesis.cpp
        Transposer/errorHandling/errorTypes/MissingParenthesis.h
        Transposer/errorHandling/errorTypes/MissingBrace.cpp
        Transposer/errorHandling/errorTypes/MissingBrace.h
        Transposer/errorHandling/errorTypes/UnexpectedToken.cpp
        Transposer/errorHandling/errorTypes/UnexpectedToken.h
        Transposer/errorHandling/errorTypes/InvalidExpression.cpp
        Transposer/errorHandling/errorTypes/InvalidExpression.h
        Transposer/errorHandling/errorTypes/MissingIdentifier.cpp
        Transposer/errorHandling/errorTypes/MissingIdentifier.h
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
        Transposer/errorHandling/errorTypes/IdentifierTaken.cpp
        Transposer/errorHandling/errorTypes/IdentifierTaken.h
        Transposer/other/Utils.cpp
        Transposer/other/Utils.h
        Transposer/AST/statements/VarDeclStmt.cpp
        Transposer/AST/statements/VarDeclStmt.h
        Transposer/AST/statements/AssignmentStmt.cpp
        Transposer/AST/statements/AssignmentStmt.h
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
        Transposer/errorHandling/errorTypes/WrongReturnType.cpp
        Transposer/errorHandling/errorTypes/WrongReturnType.h
        Transposer/errorHandling/errorTypes/NoReturnStmt.cpp
        Transposer/errorHandling/errorTypes/NoReturnStmt.h
        Transposer/errorHandling/errorTypes/MainOverride.cpp
        Transposer/errorHandling/errorTypes/MainOverride.h
        Transposer/errorHandling/errorTypes/NoMain.cpp
        Transposer/errorHandling/errorTypes/NoMain.h
        Transposer/errorHandling/errorTypes/InvalidMainArgs.cpp
        Transposer/errorHandling/errorTypes/InvalidMainArgs.h
        Transposer/errorHandling/errorTypes/InvalidMainReturnType.cpp
        Transposer/errorHandling/errorTypes/InvalidMainReturnType.h
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
        Transposer/errorHandling/errorTypes/ElseWithoutIf.cpp
        Transposer/errorHandling/errorTypes/ElseWithoutIf.h
        Transposer/AST/statements/ArrayDeclStmt.cpp
        Transposer/AST/statements/ArrayDeclStmt.h
        Transposer/symbols/Type/ArrayType.cpp
        Transposer/symbols/Type/ArrayType.h
        Transposer/AST/statements/WhileStmt.cpp
        Transposer/AST/statements/WhileStmt.h
        Transposer/AST/statements/BreakStmt.cpp
        Transposer/AST/statements/BreakStmt.h
        Transposer/AST/statements/expression/ArraySlicingExpr.cpp
        Transposer/AST/statements/expression/ArraySlicingExpr.h
        Transposer/AST/statements/expression/ArrayIndexingExpr.cpp
        Transposer/AST/statements/expression/ArrayIndexingExpr.h
        Transposer/AST/abstract/Call.h
        Transposer/AST/statements/SwitchStmt.cpp
        Transposer/AST/statements/SwitchStmt.h
        Transposer/AST/statements/CaseStmt.cpp
        Transposer/AST/statements/CaseStmt.h
        Transposer/AST/statements/ContinueStmt.cpp
        Transposer/AST/statements/ContinueStmt.h
        Transposer/AST/statements/ForStmt.cpp
        Transposer/AST/statements/ForStmt.h
        Transposer/errorHandling/errorTypes/HowDidYouGetHere.cpp
        Transposer/errorHandling/errorTypes/HowDidYouGetHere.h
        Transposer/errorHandling/errorTypes/StmtNotBreakable.cpp
        Transposer/errorHandling/errorTypes/StmtNotBreakable.h
        Transposer/errorHandling/errorTypes/StmtNotContinueAble.cpp
        Transposer/errorHandling/errorTypes/StmtNotContinueAble.h
        Transposer/errorHandling/errorTypes/UnrecognizedToken.cpp
        Transposer/errorHandling/errorTypes/UnrecognizedToken.h
        Transposer/errorHandling/errorTypes/UnRecognizedIdentifier.cpp
        Transposer/errorHandling/errorTypes/UnRecognizedIdentifier.h
        Transposer/errorHandling/errorTypes/StmtInsideSwitchThatIsNotCase.cpp
        Transposer/errorHandling/errorTypes/StmtInsideSwitchThatIsNotCase.h
        Transposer/errorHandling/errorTypes/InvalidNumberLiteral.cpp
        Transposer/errorHandling/errorTypes/InvalidNumberLiteral.h
        Transposer/errorHandling/errorTypes/NoPlacementOperator.cpp
        Transposer/errorHandling/errorTypes/NoPlacementOperator.h
        Transposer/errorHandling/errorTypes/ExpectedAnExpression.cpp
        Transposer/errorHandling/errorTypes/ExpectedAnExpression.h
        Transposer/errorHandling/errorTypes/MissingPipe.cpp
        Transposer/errorHandling/errorTypes/MissingPipe.h
)

# In order to call the exe from the cmd
target_link_options(Transposer PRIVATE -static -static-libgcc -static-libstdc++)

include(FetchContent)


# Boost

find_package(Boost REQUIRED regex)
target_link_libraries(Transposer PRIVATE Boost::boost)