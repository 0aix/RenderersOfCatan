#include "draw.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"

#define CATAN_GLFW_INIT_FAILED 0
#define CATAN_GLFW_CREATE_WINDOW_FAILED 1
#define CATAN_GLEW_INIT_FAILED 2

namespace Catan
{
    namespace Draw
    {
        const char* const filenames[] =
        {
            "./tiles/sheep.tga",
            "./tiles/brick.tga",
            "./tiles/mountain.tga",
            "./tiles/wheat.tga",
            "./tiles/forest.tga",
            "./tiles/water.tga",
            "./tiles/desert.tga",
            "./tiles/gold.tga"
        };
        const GLfloat tile_vertices[] = {
            -0.5f, 0.8660254f, 0.0f,
            0.5f, 0.8660254f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.5f, -0.8660254f, 0.0f,
            -0.5f, -0.8660254f, 0.0f,
            -1.0f, 0.0f, 0.0f
        };
        const GLfloat tile_uv[] = {
            4257.0f / 7040.0f, 7246.0f / 10204.0f,
            5303.0f / 7040.0f, 7209.0f / 10204.0f,
            5865.0f / 7040.0f, 8099.0f / 10204.0f,
            5374.0f / 7040.0f, 9015.0f / 10204.0f,
            4324.0f / 7040.0f, 9056.0f / 10204.0f,
            3761.0f / 7040.0f, 8169.0f / 10204.0f
        };

        GLuint tiles[8];
        GLuint vertexarray;
        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint diffuseShader;
        GLuint textureShader;
        GLuint texture;
        GLuint framebuffer;

        bool Initialize()
        {
            try
            {
                if (!glfwInit())
                    throw CATAN_GLFW_INIT_FAILED;
                
                glfwWindowHint(GLFW_SAMPLES, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

                GLFWwindow* window = glfwCreateWindow(512, 512, "Renderers of Catan", NULL, NULL);
                if (window == NULL)
                    throw CATAN_GLFW_CREATE_WINDOW_FAILED;
                glfwMakeContextCurrent(window);

                glewExperimental = true;
                if (glewInit() != GLEW_OK)
                    throw CATAN_GLEW_INIT_FAILED;
                
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);

                LoadAll();

                return true;
            }
            catch (int error)
            {
                switch (error)
                {
                case CATAN_GLFW_CREATE_WINDOW_FAILED:
                    glfwTerminate();
                case CATAN_GLFW_INIT_FAILED:
                    break;
                }
            }
            return false;
        }

        void Uninitialize()
        {
            

            glfwTerminate();
        }

        void LoadAll()
        {
            // Load tile textures
            glGenTextures(sizeof(tiles), tiles);

            char* buffer = new char[2048 * 2048 * 3];

            for (int i = 0; i < sizeof(tiles); i++)
            {
                std::fstream fs(filenames[i], std::fstream::in);

                fs.seekg(18);
                fs.read(buffer, 2048 * 2048 * 3);
                fs.close();

                glBindTexture(GL_TEXTURE_2D, tiles[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            delete buffer;

            // Load vertex array
            glGenVertexArrays(1, &vertexarray);
            glBindVertexArray(vertexarray);

            // Load vertex buffer
            glGenBuffers(1, &vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);

            // Load uv buffer
            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_uv), tile_uv, GL_STATIC_DRAW);

            // Load shaders
            diffuseShader = LoadShaders("./shaders/diffuse.vs", "./shaders/diffuse.fs");
            textureShader = LoadShaders("./shaders/texture.vs", "./shaders/texture.fs");
            texture = glGetUniformLocation(textureShader, "sampler");

            // Load target frame buffer
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            
            GLenum DrawBuffer = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, &DrawBuffer);
        }

        void Render()
        {
            // Compute width and height

            // Set up render target and depth buffer
            GLuint rendertarget;
            glGenTextures(1, &rendertarget);
            glBindTexture(GL_TEXTURE_2D, rendertarget);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            GLuint depthrenderbuffer;
            glGenRenderbuffers(1, &depthrenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendertarget, 0);

            // Render




            // Pass in render target as texture
            Preview();

            // Release render target and depth buffer
            glDeleteTextures(1, &rendertarget);
            glDeleteRenderbuffers(1, &depthrenderbuffer);
        }

        void DrawTile(float x, float y, float length, float margin, int type)
        {
            // Draw tile outline
            

            // 
        }

        void Preview()
        {

        }
    }
}