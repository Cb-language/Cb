cmake_minimum_required(VERSION 4.1)
project(ObjTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ObjTest ObjFiles/main.cpp
        ObjFiles/Object.h
        ObjFiles/Object.cpp
        ObjFiles/Primitive.h
        ObjFiles/Primitive.tpp
        ObjFiles/String.cpp
        ObjFiles/String.h
        ObjFiles/Array.h
        ObjFiles/Array.tpp
        ObjFiles/SafePtr.tpp
        ObjFiles/SafePtr.h
        ObjFiles/Utils.tpp
        ObjFiles/Utils.h)
