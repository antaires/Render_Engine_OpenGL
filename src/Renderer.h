#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "Helpers.h"
#include "VertexArray.h"

class VertexArray;
class IndexBuffer;

// could make this a singleton, but you might want multiple instances of the render
class Renderer {
public:
    // TODO: more traditional to take in a material rather than a shader for Draw()
    // in this case, the renderer sets up the shader / uniforms on its own
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
private:
};
