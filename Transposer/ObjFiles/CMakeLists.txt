cmake_minimum_required(VERSION 4.1)
project(ObjTest)

set(CMAKE_CXX_STANDARD 20)

add_executable(ObjTest main.cpp
        Object.h
        Object.cpp
        Primitive.h
        Primitive.tpp
        String.cpp
        String.h
        Array.h
        Array.tpp
        SafePtr.tpp
        SafePtr.h
        Utils.tpp
        Utils.h)
