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
        Transposer/Token.cpp
        Transposer/Token.h
        Transposer/Tokenizer.cpp
        Transposer/Tokenizer.h
        Transposer/Statement.h)

include(FetchContent)


# Boost

find_package(Boost REQUIRED regex)
target_link_libraries(Transposer PRIVATE Boost::boost)