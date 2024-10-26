cmake_minimum_required(VERSION 3.29)
project(ConnectFour)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

add_executable(ConnectFour main.cpp
        Board.hpp
        Board.cpp
        Game.hpp
)
