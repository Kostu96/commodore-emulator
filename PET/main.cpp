#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"
#include "shader_sources.inl"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glw/glw.hpp>
#include <glw/error_callback.inl>

#include "roms/charset.inl"

#include <thread>

#include <signal.h>

glw::Shader* pointShader = nullptr;
struct CharVertex {
    u16 x, y;
    u32 color;
} charVertices[8][8];
glw::VertexBuffer* charVBO = nullptr;
glw::VertexArray* charVAO = nullptr;

glw::Shader* textureShader = nullptr;
struct TextureVertex {
    f32 x, y;
    f32 u, v;
} textureVertices[] = {
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.f, 1.f, 0.f,
     1.f,  1.f, 1.f, 1.f,
    -1.f,  1.f, 0.f, 1.f
};
u8 textureIndices[] = {
    0, 1, 2,
    2, 3, 0
};
glw::VertexBuffer* textureVBO = nullptr;
glw::IndexBuffer* textureIBO = nullptr;
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

    textureVBO = new glw::VertexBuffer{ textureVertices, sizeof(textureVertices) };
    textureIBO = new glw::IndexBuffer{ textureIndices, sizeof(textureIndices) / sizeof(u8), glw::IndexBuffer::IndexType::U8 };
    glw::VertexLayout texturelayout{{
        { glw::LayoutElement::DataType::F32_2 },
        { glw::LayoutElement::DataType::F32_2 }
    }};
    textureVAO = new glw::VertexArray{ *textureVBO, texturelayout };
    textureVAO->setIndexBuffer(*textureIBO);
}

void shutdown()
{
    delete textureVAO;
    delete textureIBO;
    delete textureVBO;

    delete charVAO;
    delete charVBO;

    delete textureShader;
    delete pointShader;
}

void updateScreen(u16 offset, u8 data)
{
    if (offset >= 1000) return;

    [[maybe_unused]] const u8* char_data = charset[data];
    //if (data != 32)
        //__debugbreak();
        //raise(SIGTRAP);

    for (u8 row = 0; row < 8; row++)
    {
        for (u8 col = 0; col < 8; col++)
        {
            charVertices[row][col].x = (offset % 40) * 8 + col;
            charVertices[row][col].y = (offset / 40) * 8 + row;
            u8 bit = (0x80 >> col);
            charVertices[row][col].color = (char_data[row] & bit) ? 0xFF558371 : 0xFFDBF5E9;
        }
    }

    charVBO->bind();
    charVBO->setData(charVertices, sizeof(charVertices));
    
    charVAO->bind();
    pointShader->bind();
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

    constexpr u32 width = 40 * 8 * 2 + 10;
    constexpr u32 height = 25 * 8 * 2 + 10;
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

    glw::Framebuffer* FBO = new glw::Framebuffer{
        glw::Framebuffer::Properties{ 40 * 8, 25 * 8, 1, {
            glw::TextureSpecification{
                glw::TextureFormat::RGBA8,
                glw::TextureFilter::Nearest,
                glw::TextureFilter::Nearest,
                glw::TextureWrapMode::Clamp
            }
        }}
    };
    FBO->bind();
    glViewport(0, 0, 40 * 8, 25 * 8);
    glActiveTexture(GL_TEXTURE0);
    FBO->getAttachments()[0].bind(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (size_t i = 0; i < 128; ++i)
            cpu.clock();

        FBO->unbind();
        glViewport(5, 5, 40 * 8 * 2, 25 * 8 * 2);
        textureVAO->bind();
        textureShader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        glFinish();
        FBO->bind();
        glViewport(0, 0, 40 * 8, 25 * 8);
        //std::this_thread::sleep_for(std::chrono::microseconds(128));
    }

    delete FBO;
    shutdown();
    glfwTerminate();
    return 0;
}
