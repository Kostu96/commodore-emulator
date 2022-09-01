cmake_minimum_required(VERSION 3.16)

project(cpu6502-proj LANGUAGES CXX)

add_library(cpu6502 STATIC
    cpu6502.cpp
    cpu6502.hpp
    type_aliases.hpp
)

target_include_directories(cpu6502 SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
