#include "shader.hpp"

#include <glad/gl.h>

#include <iostream>

static u32 shaderTypeToOGLShaderType(Shader::Type type)
{
    switch (type)
    {
    case Shader::Type::Vertex:   return GL_VERTEX_SHADER;
    case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
    }

    return static_cast<u32>(-1);
}

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    u32 vs = compileShader(vertexShaderSource, Type::Vertex);
    u32 fs = compileShader(fragmentShaderSource, Type::Fragment);
    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);

    glLinkProgram(m_id);
    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        int maxLength = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[maxLength];
        glGetProgramInfoLog(m_id, maxLength, nullptr, infoLog);
        std::cerr << "Shader program linking failed!\n" << infoLog << '\n';
        glDeleteProgram(m_id);
    }

    glDetachShader(m_id, vs);
    glDetachShader(m_id, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::bind() const
{
    glUseProgram(m_id);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

u32 Shader::compileShader(const char* source, Type type) const
{
    u32 id = glCreateShader(shaderTypeToOGLShaderType(type));
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);
    int ret;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        int maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[maxLength];
        glGetShaderInfoLog(id, maxLength, nullptr, infoLog);
        std::cerr << "Shader compilation failed!\n" << infoLog << '\n';
        delete[] infoLog;
    };

    return id;
}