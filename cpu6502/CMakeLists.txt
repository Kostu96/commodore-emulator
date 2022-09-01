cmake_minimum_required(VERSION 3.16)

project(cpu6502-proj LANGUAGES CXX)

add_library(cpu6502 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu6502.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu6502.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/type_aliases.hpp
)

target_include_directories(cpu6502 SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(test6502_functional
    ${CMAKE_CURRENT_SOURCE_DIR}/test_functional.cpp
)

target_link_libraries(test6502_functional PRIVATE
    cpu6502
)

add_executable(test6502_interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/test_interrupt.cpp
)

target_link_libraries(test6502_interrupt PRIVATE
    cpu6502
)

set_target_properties(
    test6502_functional
    test6502_interrupt
    PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
