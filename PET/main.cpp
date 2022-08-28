#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"
#include "shader_sources.inl"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glw/buffers.hpp>
#include <glw/framebuffer.hpp>
#include <glw/shader.hpp>
#include <glw/vertex_array.hpp>
#include <glw/error_callback.inl>

#include "roms/charset.inl"

#include <thread>

#include <signal.h>

glw::Shader* pointShader = nullptr;
struct CharVertex
{
    u16 x, y;
    u32 color;
} charVertices[8][8];
glw::VertexBuffer* charVBO = nullptr;
glw::VertexArray* charVAO = nullptr;

glw::Shader* textureShader = nullptr;
struct TextureVertex
{
    f32 x, y;
    f32 u, v;
} textureVertices[] = {
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.f, 1.f, 0.f,
     1.f,  1.f, 1.f, 1.f,
    -1.f,  1.f, 0.f, 1.f
};
glw::VertexBuffer* textureVBO = nullptr;
glw::VertexArray* textureVAO = nullptr;

void init()
{
    pointShader = new glw::Shader{};
    pointShader->createFromSource(pointVertSource, pointFragSource);
    textureShader = new glw::Shader{};
    textureShader->createFromSource(textureVertSource, textureFragSource);

    charVBO = new glw::VertexBuffer{ sizeof(charVertices) };

    glw::VertexLayout pointLayout{{
        { glw::LayoutElement::DataType::U16_2, false },
        { glw::LayoutElement::DataType::U8_4, true, true }
    }};
    charVAO = new glw::VertexArray{ *charVBO, pointLayout };

    glw::VertexLayout texturelayout{{
        { glw::LayoutElement::DataType::F32_2 },
        { glw::LayoutElement::DataType::F32_2 }
    }};
    textureVAO = new glw::VertexArray{ *textureVBO, texturelayout };
}

void shutdown()
{
    delete textureVAO;
    delete charVAO;

    delete textureShader;
    delete pointShader;
}

void updateScreen(u16 address, u8 data)
{
    [[maybe_unused]] const u8* char_data = charset[data];
    //if (data != 32)
        //__debugbreak();
        //raise(SIGTRAP);

    for (u8 row = 0; row < 8; row++)
    {
        for (u8 col = 0; col < 8; col++)
        {
            charVertices[row][col].x = (address % 40) * 8 + col;
            charVertices[row][col].y = (address / 40) * 8 + row;
            u8 bit = (0x80 >> col);
            charVertices[row][col].color = (char_data[row] & bit) ? 0xFF558371 : 0xFFDBF5E9;
        }
    }

    charVBO->setData(charVertices, sizeof(charVertices));
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

    constexpr u32 width = 40 * 8;
    constexpr u32 height = 25 * 8;
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

    init();

    charVBO->bind();

    charVAO->bind();

    pointShader->bind();

    //glw::Framebuffer* FBO = new glw::Framebuffer{
    //    glw::Framebuffer::Properties{ 40 * 8, 25 * 8, 1, {
    //        glw::TextureFormat::RGBA8,
    //        glw::TextureFilter::Linear,
    //        glw::TextureFilter::Nearest
    //    }}
    //};
    //FBO->bind();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (size_t i = 0; i < 64; ++i)
            cpu.clock();

        //FBO->unbind();

        glFinish();
        //FBO->bind();
        //std::this_thread::sleep_for(std::chrono::microseconds(128));
    }

    //delete FBO;
    delete charVBO;

    shutdown();
    glfwTerminate();
    return 0;
}
