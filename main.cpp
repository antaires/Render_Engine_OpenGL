//
//  main.cpp
//  Experiments_in_OpenGL
//
//  Created by Valia O'Donnell on 16/04/2020.
//  Copyright © 2020 Antaires. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// window width
const GLint WIDTH = 800, HEIGHT = 600;

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// would use C file system for speed typically, but for experiments ok to use c++
static ShaderProgramSource ParseShader(const std::string& filepath)
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

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // same as &source[0]
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    // error handling (syntax etc)
    // retreive result of compile
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // i = int v = vector
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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // provide openGL with actual shader source code and have openGL compile it
    // and link vertex and fragement shaders
    unsigned int program = glCreateProgram(); // could also use glUnt (but this is better for multiple UIs
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

int main() {

    // GLFW init
    if (!glfwInit()){
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);  // can't resize
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "window name", nullptr, nullptr);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window){
        std::cout<<"Failed to create GLFW window" <<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // GLEW init
    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK){
        std::cout<<"Failed to initialize GLEW"<<std::endl;
        return -1;
    }
    std::cout<<"\nVersion: "<<glGetString(GL_VERSION)<<std::endl;
    glViewport(0, 0, screenWidth, screenHeight);
    
    float position[6] = {
      -0.5f, -0.5f,
       0.0f,  0.5f,
       0.5f, -0.5f,
    };
    
    // (me) to fix mac issue - not drawing
    unsigned int vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);
    
    // define a buffer
    unsigned int buffer;
    glGenBuffers(1, &buffer ); // gives us back an id
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // bind the buffer (bound is the one drawn)
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW); // put data in buffer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    // create shader
    ShaderProgramSource source = ParseShader("/Users/valiaodonnell/Documents/openGL/Experiments_in_OpenGL/Experiments_in_OpenGL/res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader); // bind shader
    
    // GAME LOOP
    while (!glfwWindowShouldClose(window)){
        //glClearColor(0.2f, 0.3f, 0.3f, 0.5f); // 1 is fully visible alpha
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    // clean up shader
    glDeleteProgram(shader);
    
    // CLEAN UP
    glfwTerminate();
    
    return 0;
}

























