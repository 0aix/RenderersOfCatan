#include <stdio.h>
#include <stdlib.h>
#include "draw.h"

int main()
{
    const int width = 512;
    const int height = 512;

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    glfwSetErrorCallback(error_callback);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(width, height, "Renderers of Catan", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "blah\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "wah\n");
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint tiles = loadTiles();
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
/*
    static const GLfloat g_vertex_buffer_data[] = {
        0.25f, 0.9330127f, 0.0f,
        0.75f, 0.9330127f, 0.0f,
        1.0f, 0.5f, 0.0f,
        0.75f, 0.0669872f, 0.0f,
        0.25f, 0.0669872f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
*/
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, 0.8660254f, 0.0f,
        0.5f, 0.8660254f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.5f, -0.8660254f, 0.0f,
        -0.5f, -0.8660254f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    static const GLfloat g_uv_buffer_data[] = {
        4257.0f / 7040.0f, 7246.0f / 10204.0f,
        5303.0f / 7040.0f, 7209.0f / 10204.0f,
        5865.0f / 7040.0f, 8099.0f / 10204.0f,
        5374.0f / 7040.0f, 9015.0f / 10204.0f,
        4324.0f / 7040.0f, 9056.0f / 10204.0f,
        3761.0f / 7040.0f, 8169.0f / 10204.0f
    };

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);    

    //GLuint programID = LoadShaders("basic.vs", "basic.fs");
    GLuint programID = LoadShaders("texture.vs", "texture.fs");
    GLuint textureID = glGetUniformLocation(programID, "sampler");

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "crap\n");
        return -1;
    }
    
    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, 2048, 2048);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tiles);
    glUniform1i(textureID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    GLubyte* pixels = new GLubyte[2048 * 2048 * 3];
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    //glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    std::fstream fs("./output.tga", std::fstream::in | std::fstream::out | std::fstream::trunc);

    if (!fs.is_open())
        return -1;

    char header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2048 & 0xFF, (2048 & 0xFF00) >> 8, 2048 & 0xFF, (2048 & 0xFF00) >> 8, 24, 0 };

    fs.write(header, sizeof(header));
    fs.write((char*)pixels, 2048 * 2048 * 3);

    fs.close();

    delete pixels;
/*
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tiles);
        glUniform1i(textureID, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size : U+V => 2
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
            glfwWindowShouldClose(window) == 0);
*/
    glDeleteTextures(1, &tiles);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();

    return 0;
}