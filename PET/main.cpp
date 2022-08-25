#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"

#include "opengl/buffers.hpp"
#include "opengl/shader.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "roms/charset.inl"

struct Vertex
{
    u16 x, y;
    u32 color;
};

Vertex vertices[8][8];
VertexBuffer* VBO;

void updateScreen(u16 address, u8 data)
{
    [[maybe_unused]] const u8* char_data = charset[data];
    if (data != 32)
        __debugbreak();

    for (u8 char_row = 0; char_row < 8; char_row++)
    {
        for (u8 pixel = 0; pixel < 8; pixel++)
        {
            vertices[char_row][pixel].x = address % 40;
            vertices[char_row][pixel].y = address / 40;
            vertices[char_row][pixel].color = (*char_data >> (7 - pixel)) == 1 ? 0xFFFFFFFF : 0x00FF00FF;
        }
    }

    VBO->setData(vertices, sizeof(vertices));
    glDrawArrays(GL_POINTS, 0, 8 * 8);
}

static void GLAPIENTRY openGlErrorHandler(
    GLenum source,
    GLenum type,
    GLuint /*id*/,
    GLenum /*severity*/,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/)
{
    const char* sourceStr = "";
    switch (source) {
    case GL_DEBUG_SOURCE_API: sourceStr = "Call to the OpenGL API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "Call to a window-system API"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Compiler for a shading language"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "Application associated with OpenGL"; break;
    case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "User of this application"; break;
    case GL_DEBUG_SOURCE_OTHER: sourceStr = "Unknown"; break;
    }

    const char* typeStr = "";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "Undefined behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER: typeStr = "Command stream annotation"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
    case GL_DEBUG_TYPE_POP_GROUP: typeStr = "User defined"; break;
    case GL_DEBUG_TYPE_OTHER: typeStr = "Unknown"; break;
    }

    std::cerr << "OpenGL debug | " << sourceStr << " | Type: " << typeStr << '\n' << message;
}

int main()
{
    MemoryMap memoryMap{ updateScreen };
    CPU6502 cpu{ memoryMap };

    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        std::terminate();
    }

    constexpr u32 width = 40 * 8 * 3;
    constexpr u32 height = 25 * 8 * 3;
    GLFWwindow* window = glfwCreateWindow(width, height, "Commodore PET", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!\n";
        std::terminate();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "GLAD loader failed!\n";
        std::terminate();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openGlErrorHandler, nullptr);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    
    glPointSize(3.f);

    u32 VAO;
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    VBO = new VertexBuffer{ sizeof(vertices) };
    VBO->bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_SHORT,
        GL_TRUE,
        sizeof(Vertex),
        0
    ); // position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
        (const void*)(sizeof(u16) * 2)
    ); // color

    const char* vertSource =
R"(
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = vec4(a_Position, 0.0, 1.0);
}
)";

    const char* fragSource =
        R"(
#version 450 core

layout(location = 0) in vec4 v_Color;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = v_Color;
}
)";

    Shader shader{ vertSource, fragSource };
    shader.bind();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (size_t i = 0; i < 128; ++i)
            cpu.clock();

        glfwSwapBuffers(window);
    }

    delete VBO;
    glfwTerminate();
    return 0;
}
