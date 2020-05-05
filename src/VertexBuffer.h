#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

class VertexBuffer {
private:
    unsigned int m_RendererID; // openGL needs numeric id to track what object is allocated
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();
    
    // todo add lock/ unlock, changes while streaming
    
    void Bind() const;
    void Unbind() const;
};
