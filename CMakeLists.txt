cmake_minimum_required(VERSION 3.20)

project(commodore-emulator LANGUAGES CXX)
include(scripts/cmake/base_configuration.cmake)
include(scripts/cmake/compiler_warnings.cmake)

add_subdirectory(PET)
