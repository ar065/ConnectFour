cmake_minimum_required(VERSION 3.29)
project(ConnectFour)

set(CMAKE_CXX_STANDARD 23)

add_executable(ConnectFour main.cpp
        Board.hpp
        Board.cpp
        Game.hpp
)
