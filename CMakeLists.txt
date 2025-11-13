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
        Transposer/FileManager.cpp
        Transposer/FileManager.h
        Transposer/Source.cpp
        Transposer/token/Token.cpp
        Transposer/token/Token.h
        Transposer/token/Tokenizer.cpp
        Transposer/token/Tokenizer.h
        Transposer/AST/Statement/Statement.h
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
        Transposer/parser/parser.h)

include(FetchContent)


# Boost

find_package(Boost REQUIRED regex)
target_link_libraries(Transposer PRIVATE Boost::boost)