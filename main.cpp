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

#include "src/Renderer.h"
#include "src/VertexBuffer.h"
#include "src/IndexBuffer.h"
#include "src/VertexArray.h"
#include "src/Shader.h"
#include "src/Texture.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// window width
const GLint WIDTH = 800, HEIGHT = 600;

int main() {

    // GLFW init
    if (!glfwInit()){
        return -1;
    }
    
    // create with core profile
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
    
    {
        float position[] = {
          -0.5f, -0.5f, 0.0f, 0.0f, // 0
           0.5f, -0.5f, 1.0f, 0.0f, // 1
           0.5f,  0.5f, 1.0f, 1.0f, // 2
          -0.5f,  0.5f, 0.0f, 1.0f  // 3
        };
        
        
        // index buffer
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        
        // blending alpha pixels
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));
        
        VertexArray va;
        VertexBuffer vb(position, 4 * 4 * sizeof(float));
        
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);
        
        // pass index buffer to GPU
        IndexBuffer ib(indices, 6);
        
        // create shader
        Shader shader("/Users/valiaodonnell/Documents/openGL/Experiments_in_OpenGL/Experiments_in_OpenGL/res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
        
        // texture
        Texture texture("/Users/valiaodonnell/Documents/openGL/Experiments_in_OpenGL/Experiments_in_OpenGL/res/textures/testTexture.png");
        int textureSlot = 0;
        texture.Bind(textureSlot);
        shader.SetUniform1i("u_Texture", textureSlot);
        
        
        // unbind everything
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();
        
        Renderer renderer;
        
        // GAME LOOP
        float red = 0.0f;
        float increment = 0.05f;
        while (!glfwWindowShouldClose(window)){
        
            renderer.Clear();
            
            // bind
            shader.Bind();
            shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);// set uniform in fragment shader, set PER DRAW call

            renderer.Draw(va, ib, shader);
            
            // modulate color
            if (red > 1.0f)
                increment = -0.05f;
            else if (red < 0.0f)
                increment = 0.05f;
            red += increment;
            
            GLCall(glfwSwapBuffers(window));
            
            GLCall(glfwPollEvents());
        }
    }
    
    // CLEAN UP
    GLCall(glfwTerminate());
    
    return 0;
}

























