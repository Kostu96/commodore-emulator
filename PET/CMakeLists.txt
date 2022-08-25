set(PET_TARGET_NAME pet)

set(PET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/buffers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/buffers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/basica.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/basicb.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/basicc.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/charset.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/editor4.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/kernal2.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/roms/kernal4.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu6502.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu6502.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/type_aliases.hpp
)

add_executable(${PET_TARGET_NAME} ${PET_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX source FILES ${PET_SOURCES})

set_target_warnings(${PET_TARGET_NAME})

target_compile_options(${PET_TARGET_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/MP /fp:fast /external:anglebrackets /external:W1>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

target_link_libraries(${PET_TARGET_NAME} PRIVATE
    glad
    glfw
)

set_target_properties(${PET_TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PET_TARGET_NAME}>
)

#add_custom_command(
#    TARGET ${PET_TARGET_NAME}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PET_TARGET_NAME}>/assets
#)
