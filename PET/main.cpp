#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glw/buffers.hpp>
#include <glw/shader.hpp>
#include <glw/vertex_array.hpp>
#include <glw/error_callback.inl>

#include "roms/charset.inl"

#include <thread>

struct Vertex
{
    u16 x, y;
    u32 color;
};

Vertex vertices[8][8];
glw::VertexBuffer* VBO;

void updateScreen(u16 address, u8 data)
{
    [[maybe_unused]] const u8* char_data = charset[data];
    //if (data != 32)
    //    __debugbreak();

    for (u8 row = 0; row < 8; row++)
    {
        for (u8 col = 0; col < 8; col++)
        {
            vertices[row][col].x = (address % 40) * 24 + col * 3;
            vertices[row][col].y = (address / 40) * 24 + row * 3;
            vertices[row][col].color = (char_data[row] >> (7 - col)) == 1 ? 0xFF558371 : 0xFFDBF5E9;
        }
    }

    VBO->setData(vertices, sizeof(vertices));
    glDrawArrays(GL_POINTS, 0, 8 * 8);
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
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
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
    glDebugMessageCallback(OGLErrorCallback, nullptr);
    
    glPointSize(3.f);

    VBO = new glw::VertexBuffer{ sizeof(vertices) };
    VBO->bind();

    glw::VertexLayout layout({
        { glw::LayoutElement::DataType::U16_2, false },
        { glw::LayoutElement::DataType::U8_4, true, true }
    });

    glw::VertexArray VAO{ *VBO, layout };
    VAO.bind();

    const char* vertSource =
R"(
#version 450 core

layout(location = 0) in uvec2 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 v_Color;

void main()
{
    float xpos = float(a_Position.x) / (40.0 * 8.0 * 3.0 * 0.5) - 1.0;
	float ypos = float(a_Position.y) / (25.0 * 8.0 * 3.0 * 0.5) - 1.0;

    v_Color = a_Color;
    gl_Position = vec4(xpos, ypos, 0.0, 1.0);
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

    glw::Shader shader{};
    shader.createFromSource(vertSource, fragSource);
    shader.bind();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (size_t i = 0; i < 64; ++i)
            cpu.clock();

        glFinish();
        //std::this_thread::sleep_for(std::chrono::microseconds(128));
    }

    delete VBO;
    glfwTerminate();
    return 0;
}
