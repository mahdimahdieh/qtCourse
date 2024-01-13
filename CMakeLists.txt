cmake_minimum_required(VERSION 3.26)
project(course)

set(CMAKE_CXX_STANDARD 17)

add_executable(course main.cpp
        tinyxml2.cpp
        class.xml CMakeLists.txt CMakeLists.txt.user
        tinyxml2.h)
