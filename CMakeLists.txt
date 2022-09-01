cmake_minimum_required(VERSION 3.20)

project(commodore-emulator LANGUAGES CXX)
include(scripts/cmake/base_configuration.cmake)
include(scripts/cmake/compiler_warnings.cmake)

add_subdirectory(third_party)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
add_subdirectory(cpu6502)
add_subdirectory(PET)
