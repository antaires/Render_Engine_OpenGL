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

#define ASSERT(x) if (!(x)) __builtin_trap()
#define GLCall(x) GLClearError(); x; ASSERT( GLLogCall(#x, __FILE__, __LINE__) )

// window width
const GLint WIDTH = 800, HEIGHT = 600;

static void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    // will print out decimal error code, but need to convert to hexadecimal to check header file errors
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "):" << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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
    
    glfwSwapInterval(1); // limit frame rate
    
    // GLEW init
    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK){
        std::cout<<"Failed to initialize GLEW"<<std::endl;
        return -1;
    }
    std::cout<<"\nVersion: "<<glGetString(GL_VERSION)<<std::endl;
    glViewport(0, 0, screenWidth, screenHeight);
    
    float position[] = {
      -0.5f, -0.5f, // 0
       0.5f, -0.5f, // 1
       0.5f,  0.5f, // 2
      -0.5f,  0.5f  // 3
    };
    
    // index buffer
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // (me) to fix mac issue - not drawing
    unsigned int vertexArrayId;
    GLCall(glGenVertexArrays(1, &vertexArrayId));
    GLCall(glBindVertexArray(vertexArrayId));
    
    // define a buffer
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer )); // gives us back an id
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // bind the buffer (bound is the one drawn)
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), position, GL_STATIC_DRAW)); // put data in buffer
    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
    
    // pass index buffer to GPU
    unsigned int ibo; // index buffer object
    GLCall(glGenBuffers(1, &ibo)); // gives us back an id
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // bind the buffer (bound is the one drawn)
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW)); // put data in buffer
    
    // create shader
    ShaderProgramSource source = ParseShader("/Users/valiaodonnell/Documents/openGL/Experiments_in_OpenGL/Experiments_in_OpenGL/res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader)); // bind shader
    
    GLCall(int location = glGetUniformLocation(shader, "u_Color")); // get location of variable
    ASSERT(location != -1); // uniform not found (not always an error, but for here its ok)
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f)); // set uniform in fragment shader
    
    // GAME LOOP
    float red = 0.0f;
    float increment = 0.05f;
    while (!glfwWindowShouldClose(window)){
        //glClearColor(0.2f, 0.3f, 0.3f, 0.5f); // 1 is fully visible alpha
        
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        
        GLCall(glUniform4f(location, red, 0.3f, 0.8f, 1.0f)); // set uniform in fragment shader, set PER DRAW call 
        GLCall( glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr) ); // number of INDICES
        
        if (red > 1.0f)
            increment = -0.05f;
        else if (red < 0.0f)
            increment = 0.05f;
        red += increment;
        
        GLCall(glfwSwapBuffers(window));
        
        GLCall(glfwPollEvents());
    }
    
    // clean up shader
    GLCall(glDeleteProgram(shader));
    
    // CLEAN UP
    GLCall(glfwTerminate());
    
    return 0;
}

























