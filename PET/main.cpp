#include <iostream>

#include "memory_map.hpp"
#include "cpu6502.hpp"
#include "shader_sources.inl"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glw/glw.hpp>

#include "roms/charset.inl"

constexpr u16 PET_SCREEN_COLS = 40;
constexpr u16 PET_SCREEN_ROWS = 25;
constexpr u16 PET_SCREEN_WIDTH = PET_SCREEN_COLS * 8;
constexpr u16 PET_SCREEN_HEIGHT = PET_SCREEN_ROWS * 8;

glw::Texture* keyboardTexture = nullptr;

glw::Shader* pointShader = nullptr;
struct CharVertex {
    u16 x, y;
    u32 color;
} charVertices[1000][8][8];
glw::VertexBuffer* charVBO = nullptr;
glw::VertexArray* charVAO = nullptr;

glw::Shader* textureShader = nullptr;
struct TextureVertex {
    f32 x, y;
    f32 u, v;
} textureVertices[] = {
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.0f, 1.f, 0.f,
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
    glActiveTexture(GL_TEXTURE0);

    keyboardTexture = new glw::Texture{ "assets/PET_Keyboard.png", true };

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

    for (u16 offset = 0; offset < 1000; offset++)
        for (u8 row = 0; row < 8; row++)
            for (u8 col = 0; col < 8; col++)
            {
                charVertices[offset][row][col].x = (offset % PET_SCREEN_COLS) * 8 + col;
                charVertices[offset][row][col].y = (offset / PET_SCREEN_COLS) * 8 + row;
                charVertices[offset][row][col].color = 0xFFDBF5E9;
            }
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

    delete keyboardTexture;
}

void updateScreen(u8* vram)
{
    for (u16 offset = 0; offset < 1000; offset++)
    {
        u8 character = vram[offset];
        const u8* char_data = charset[character];
        for (u8 row = 0; row < 8; row++)
            for (u8 col = 0; col < 8; col++)
            {
                u8 bit = (0x80 >> col);
                charVertices[offset][row][col].color = (char_data[row] & bit) ? 0xFF558371 : 0xFFDBF5E9;
            }
    }
    charVBO->bind();
    charVBO->setData(charVertices, sizeof(charVertices));

    glDrawArrays(GL_POINTS, 0, 1000 * 8 * 8);
}

int main()
{
    MemoryMap memoryMap{};
    CPU6502 cpu{ memoryMap };
    memoryMap.init(cpu, updateScreen);

    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        std::terminate();
    }

    constexpr u32 width = std::max(881, PET_SCREEN_WIDTH * 2);
    constexpr u32 height = 294 + PET_SCREEN_HEIGHT * 2 + 16;
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Commodore PET", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!\n";
        std::terminate();
    }

    glfwMakeContextCurrent(window);
    glw::init(glfwGetProcAddress);

    init();

    glw::Framebuffer* FBO = new glw::Framebuffer{
        glw::Framebuffer::Properties{ PET_SCREEN_WIDTH, PET_SCREEN_HEIGHT, 1, {
            glw::TextureSpecification{
                glw::TextureFormat::RGBA8,
                glw::TextureFilter::Nearest,
                glw::TextureFilter::Nearest,
                glw::TextureWrapMode::Clamp
            }
        }}
    };
    FBO->bind();
    

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (size_t i = 0; i < 1024; ++i)
            cpu.clock();

        FBO->unbind();
        textureVAO->bind();
        textureShader->bind();
        glViewport((881 - PET_SCREEN_WIDTH * 2) / 2, 294 + 8, 40 * 8 * 2, 25 * 8 * 2);
        FBO->getAttachments()[0].bind(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        glViewport(0, 0, 881, 294);
        keyboardTexture->bind(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        glFinish();

        FBO->bind();
        charVAO->bind();
        pointShader->bind();
        glViewport(0, 0, 40 * 8, 25 * 8);
    }

    delete FBO;
    shutdown();
    glfwTerminate();
    return 0;
}
