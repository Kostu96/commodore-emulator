#pragma once
#include "../type_aliases.hpp"

class VertexBuffer final
{
public:
    VertexBuffer(const void* vertices, size_t size);
    explicit VertexBuffer(size_t size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    void setData(const void* data, size_t size) const;
private:
    u32 m_id;
};

class IndexBuffer final
{
public:
    enum class IndexType {
        U8,
        U16,
        U32
    };

    IndexBuffer(const void* indices, u32 count, IndexType type);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    u32 getCount() const { return m_count; }
    IndexType getIndexType() const { return m_indexType; }
private:
    u32 m_id;
    u32 m_count;
    IndexType m_indexType;
};
