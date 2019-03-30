cmake_minimum_required(VERSION 3.12)
project(dfadot)

set(CMAKE_CXX_STANDARD 11)

add_executable(dfadot ./src/main.cpp ./src/NFA.cpp ./src/NFA.h ./src/jsoncpp.cpp ./src/DFA.cpp ./src/DFA.h ./src/ENFA.cpp ./src/ENFA.h)