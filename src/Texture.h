#pragma once

#include "Renderer.h"
#include "Helpers.h"

class Texture
{
private:
    unsigned int m_RendererId;
    std::string m_FilePath;
    unsigned char* m_localBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string& path);
    ~Texture();
    
    void Bind(unsigned int slot = 0) const; // pick texture slot (modern GPUs roughly 32 slots, mobile have 8
    void Unbind() const;
    
    inline int GetWidth() const {return m_Width;}
    inline int GetHeight() const {return m_Height;}
};
