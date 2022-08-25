#pragma once
#include "../type_aliases.hpp"

class Shader final
{
public:
    enum class Type {
        Vertex,
        Fragment
    };

    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();

    void bind() const;
    void unbind() const;
private:
    u32 compileShader(const char* source, Type type) const;

    u32 m_id;
};
