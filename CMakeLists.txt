cmake_minimum_required(VERSION 3.12)
project(dfadot)

set(CMAKE_CXX_STANDARD 11)

add_executable(dfadot main.cpp NFA.cpp NFA.h jsoncpp.cpp)