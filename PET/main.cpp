#include <iostream>

#include "cpu6502.hpp"
#include "io.hpp"
#include "keyboard.hpp"
#include "shader_sources.inl"

#include <glad/gl.h>
#include <glw/glw.hpp>
#include <GLFW/glfw3.h>

#include "roms/charset.inl"

constexpr u16 PET_SCREEN_COLS = 40;
constexpr u16 PET_SCREEN_ROWS = 25;
constexpr u16 PET_SCREEN_WIDTH = PET_SCREEN_COLS * 8;
constexpr u16 PET_SCREEN_HEIGHT = PET_SCREEN_ROWS * 8;

#pragma region OGLStuff
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
#pragma endregion

#define ROM_SERIES1

static struct {
    u8 memory[0x2000];

    u8 read(u16 address) const { return memory[address]; }
    void write(u16 address, u8 data) { memory[address] = data; }
} RAM;

static struct {
    u8 memory[0x400];

    u8 read(u16 address) const { return memory[address]; }
    void write(u16 address, u8 data) { memory[address] = data; }

    u8& operator[](u16 offset) { return memory[offset]; }
} VRAM;

static struct {
    inline
#if defined(ROM_SERIES1)
#include "roms/basic1_C000.inl"
#endif
#if defined(ROM_SERIES2)
#include "roms/basic2_C000.inl"
#endif
#if defined(ROM_SERIES4)
#include "roms/basic4_B000.inl"
#endif

    u8 read(u16 address) const { return BASIC[address]; }
} BASIC;

static struct {
    inline
#if defined(ROM_SERIES1)
#include "roms/editor1_E000.inl"
#endif
#if defined(ROM_SERIES2)
#include "roms/editor2_E000.inl"
#endif
#if defined(ROM_SERIES4)
#include "roms/editor4_E000.inl"
#endif

    u8 read(u16 address) const { return EDITOR[address]; }
} EDITOR;

static struct {
    inline
#if defined(ROM_SERIES1)
#include "roms/kernal1_F000.inl"
#endif
#if defined(ROM_SERIES2)
#include "roms/kernal2_F000.inl"
#endif
#if defined(ROM_SERIES4)
#include "roms/kernal4_F000.inl"
#endif

    u8 read(u16 address) const { return KERNAL[address]; }
} KERNAL;

void updateScreen()
{
    for (u16 offset = 0; offset < 1000; offset++)
    {
        u8 character = VRAM[offset];
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
    CPU6502 cpu{};
    IO io{ cpu, updateScreen };

    cpu.map(RAM,    { 0x0000, 0x1FFF });
    cpu.map(VRAM,   { 0x8000, 0x83FF });
#if defined(ROM_SERIES4)
    cpu.map(BASIC,  { 0xB000, 0xDFFF });
#else
    cpu.map(BASIC,  { 0xC000, 0xDFFF });
#endif
    cpu.map(EDITOR, { 0xE000, 0xE7FF });
    cpu.map(io,     { 0xE800, 0xEFFF });
    cpu.map(KERNAL, { 0xF000, 0xFFFF });

    cpu.RST();

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
    glfwSetWindowUserPointer(window, &io.getKeyboard());
    glfwSetKeyCallback(window, Keyboard::glfwKeyCallback);

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

        for (size_t i = 0; i < 1024; ++i) {
            io.clock();
            cpu.CLK();
        }

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
