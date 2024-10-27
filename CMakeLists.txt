cmake_minimum_required(VERSION 3.29)
project(ConnectFour)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

add_executable(ConnectFour src/main.cpp
        src/Board.hpp
        src/Board.cpp
        src/Game.hpp
        src/BoardPrinter.hpp
        src/BoardPrinter.cpp
)
