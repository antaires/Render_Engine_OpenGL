#include "Texture.h"

#include "vender/stb_image/stb_image.h"

Texture::Texture(const std::string& path)
    : m_RendererId(0), m_FilePath(path), m_localBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
    // load image
    stbi_set_flip_vertically_on_load(1); // flips texture vertically (makes it upside down for openGL
    m_localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); // 4 for RGBA
    
    GLCall(glGenTextures(1, &m_RendererId));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererId));
    
    // set texture parameters
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    
    // give data to openGL
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
    // unbind
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    
    if (m_localBuffer) // might want to retain a copy on CPU for sampling as well
    {
        stbi_image_free(m_localBuffer);
    }
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererId));
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // define texture slot
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererId));
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
