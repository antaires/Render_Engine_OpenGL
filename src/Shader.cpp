#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath): m_FilePath(filepath), m_RendererId(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererId = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererId)); // if unsuccessful, m_RendererId is 0 and no effect
}

// would use C file system for speed typically, but for experiments ok to use c++
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    if ( stream.fail() )
    {
        std::cout<< "stream failed to open " << filepath << std::endl;
    }
    
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while( getline(stream, line) )
    {
        if(line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
                
            } else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            if (type == ShaderType::NONE)
            {
                std::cout << "Error reading shaders" << std::endl;
            }
            // add line to current shader using type as index
            std::cout<< line << std::endl;
            ss[(int)type] << line << '\n';
        }
    }
    stream.close();
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // same as &source[0]
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));
    
    // error handling (syntax etc)
    // retreive result of compile
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result)); // i = int v = vector
    if (result == GL_FALSE)
    {
        int length; // buffer size
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char)); // alloca() keeps allocation on the stack
        glGetShaderInfoLog(id, length, &length, message);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::cout<<"Failed to compile "<<shaderType<<"shader"<<std::endl;
        std::cout<<message<<std::endl;
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // provide openGL with actual shader source code and have openGL compile it
    // and link vertex and fragement shaders
    unsigned int program = glCreateProgram(); // could also use glUnt (but this is better for multiple UIs
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
    
    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererId));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value)); // set uniform in fragment shader
}


void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value)); // set uniform in fragment shader
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); // set uniform in fragment shader
}

void Shader::SetUniformMat4f(const std::string& name, glm::mat4 matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0])); 
}


int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    
    // get location of variable
    GLCall(int location = glGetUniformLocation(m_RendererId, name.c_str()));
    if (location == -1)
    {
        // a valid condition (if stripped by compiler)
        std::cout << "Warning: uniform " << name << "doesn't exist" << std::endl;
    }

    m_UniformLocationCache[name] = location;
        
    return location;
}
