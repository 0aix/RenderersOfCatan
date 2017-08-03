#include "draw.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

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
        const GLfloat tile_vertices[] = 
        {
            -0.5f, 0.8660254f,
            0.5f, 0.8660254f,
            1.0f, 0.0f,
            0.5f, -0.8660254f,
            -0.5f, -0.8660254f,
            -1.0f, 0.0f
        };
        const GLfloat tile_uv[] = 
        {
            0.25f, 0.0669873f,
            0.75f, 0.0669873f,
            1.0f, 0.5f,
            0.75f, 0.9330127f,
            0.25f, 0.9330127f,
            0.0f, 0.5f
        };
        const GLfloat preview_vertices[] =
        {
            -1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f
        };
        const GLfloat preview_uv[] = 
        {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };
        GLFWwindow* window;
        GLuint tiles[8];
        GLuint vertexarray;
        GLuint vertexbuffer[4];
        GLuint uvbuffer[2];
        GLuint diffuseShader;
        GLuint textureShader;
        GLuint texture;

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

                if ((window = glfwCreateWindow(512, 512, "Renderers of Catan", NULL, NULL)) == NULL)
                    throw CATAN_GLFW_CREATE_WINDOW_FAILED;
                glfwMakeContextCurrent(window);

                glewExperimental = true;
                if (glewInit() != GLEW_OK)
                    throw CATAN_GLEW_INIT_FAILED;

                glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

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
            glDeleteBuffers(4, vertexbuffer);
            glDeleteBuffers(2, uvbuffer);
            glDeleteTextures(8, tiles);
            glDeleteVertexArrays(1, &vertexarray);

            glDeleteProgram(diffuseShader);
            glDeleteProgram(textureShader);

            glfwTerminate();
        }

        GLuint chit;

        void LoadAll()
        {
            // Load tile textures
            char* buffer = new char[2048 * 2048 * 3];
            glGenTextures(sizeof(tiles), tiles);
            for (int i = 0; i < sizeof(tiles); i++)
            {
                std::fstream fs(filenames[i], std::fstream::in);
                fs.seekg(18);
                fs.read(buffer, 2048 * 2048 * 3);
                fs.close();

                glBindTexture(GL_TEXTURE_2D, tiles[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            delete buffer;

            /////////////////////////////////////////////////////////// TEMP
            buffer = new char[5120 * 6600 * 3];
            glGenTextures(1, &chit);
            std::fstream fs("./chits.bmp", std::fstream::in);
            fs.seekg(54);
            fs.read(buffer, 5120 * 6600 * 3);
            fs.close();

            glBindTexture(GL_TEXTURE_2D, chit);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 5120, 6600, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            delete buffer;
            ///////////////////////////////////////////////////////////

            // Load vertex array
            glGenVertexArrays(1, &vertexarray);
            glBindVertexArray(vertexarray);

            // Load circle vertices
            GLfloat* circle_vertices = new GLfloat[1024 * 2];
            float radian = 0.0f;
            for (int i = 0; i < 1024; i++)
            {
                circle_vertices[i * 2] = cos(radian);
                circle_vertices[i * 2 + 1] = sin(radian);
                radian += 2.0f * 3.141593f / 1024;
            }

            // Load vertex buffers
            glGenBuffers(4, vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(preview_vertices), preview_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1024 * 2, circle_vertices, GL_STATIC_DRAW);

            delete circle_vertices;

            ///////////////////////// TEMP
            GLfloat* chit_vertices = new GLfloat[1024 * 2];
            radian = -1.01f;
            for (int i = 0; i < 1024; i++)
            {
                float radius = 283.0f;
                chit_vertices[i * 2] = (cos(radian) * radius + 1959.0f) / 5120.0f;
                chit_vertices[i * 2 + 1] = (sin(radian) * radius + 4488.0f) / 6600.0f;
                radian += 2.0f * 3.141593f / 1024;
            }
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1024 * 2, chit_vertices, GL_STATIC_DRAW);
            delete chit_vertices;
            /////////////////////////

            // Load uv buffers
            glGenBuffers(2, uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_uv), tile_uv, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(preview_uv), preview_uv, GL_STATIC_DRAW);

            // Load shaders
            diffuseShader = LoadShaders("./shaders/diffuse.vs", "./shaders/diffuse.fs");
            textureShader = LoadShaders("./shaders/texture.vs", "./shaders/texture.fs");
            texture = glGetUniformLocation(textureShader, "sampler");

            //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        }

        void Render()
        {
            // Compute width and height


            // Set up regular and multisample FBOs, render targets, and depth buffers
            GLuint framebuffer[2];
            GLuint rendertarget[2];
            GLuint depthrenderbuffer[2];
            glGenFramebuffers(2, framebuffer);
            glGenTextures(2, rendertarget);
            glGenRenderbuffers(2, depthrenderbuffer);

            // Regular
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[0]);
            glBindTexture(GL_TEXTURE_2D, rendertarget[0]);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendertarget[0], 0);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[0]);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[0]);

            // Multisample
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[1]);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1]);
            //glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 2048, 2048, GL_TRUE);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 512, 512, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1], 0);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[1]);
            //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, 2048, 2048);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, 512, 512);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[1]);

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ////////////////////////// TEMP

            glViewportIndexedf(0.0f, 0.0f, 0.0f, 512.0f, 512.0f);

            glUseProgram(textureShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, chit);
            glUniform1i(texture, 0);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            /////////////////////////

            //DrawTile(1024.0f, 1024.0f, 400.0f, 5.0f, 0);
            //DrawTile(256.0f, 256.0f, 200.0f, 5.0f, 0);
            //DrawChit(1024.0f, 1024.0f, 200.0f, 5.0f, 0);
            //DrawChit(256.0f, 256.0f, 200.0f, 5.0f, 0);

            // Multisample blit + flip
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer[0]);
            //glBlitFramebuffer(0, 0, 2048, 2048, 0, 0, 2048, 2048, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBlitFramebuffer(0, 0, 512, 512, 0, 512, 512, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            
            // Save texture
            SaveToFile(rendertarget[0]);

            // Preview texture
            Preview(rendertarget[0]);

            // Release resources
            glDeleteTextures(2, rendertarget);
            glDeleteRenderbuffers(2, depthrenderbuffer);
            glDeleteFramebuffers(2, framebuffer);
        }

        void DrawTile(float cx, float cy, float length, float margin, int type)
        {
            // Draw tile outline
            float halfwidth = length + margin / 0.8660254f;

            glViewportIndexedf(0, cx - halfwidth, cy - halfwidth, 2.0f * halfwidth, 2.0f * halfwidth);

            glUseProgram(diffuseShader);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
            glDisableVertexAttribArray(0);
            
            // Draw tile
            glViewportIndexedf(0, cx - length, cy - length, 2.0f * length, 2.0f * length);

            glUseProgram(textureShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tiles[type]);
            glUniform1i(texture, 0);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        void DrawChit(float cx, float cy, float radius, float margin, int num)
        {
            float rradius = radius + margin;
            glViewportIndexedf(0, cx - rradius, cy - rradius, 2.0f * rradius, 2.0f * rradius);

            glUseProgram(diffuseShader);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
            glDisableVertexAttribArray(0);
        }

        void SaveToFile(GLuint textureID)
        {
            //char* buffer = new char[2048 * 2048 * 3];
            char* buffer = new char[512 * 512 * 3];
            glBindTexture(GL_TEXTURE_2D, textureID);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);

            char header[18] = { };
            header[2] = 2;
            //header[12] = 2048 & 0xFF;
            //header[13] = (2048 >> 8) & 0xFF;
            //header[14] = 2048 & 0xFF;
            //header[15] = (2048 >> 8) & 0xFF;
            header[12] = 512 & 0xFF;
            header[13] = (512 >> 8) & 0xFF;
            header[14] = 512 & 0xFF;
            header[15] = (512 >> 8) & 0xFF;
            header[16] = 24;

            std::fstream fs("./output/output.tga", std::fstream::out | std::fstream::trunc);
            fs.write(header, sizeof(header));
            //fs.write(buffer, 2048 * 2048 * 3);
            fs.write(buffer, 512 * 512 * 3);
            fs.close();
            delete buffer;
        }

        void Preview(GLuint textureID)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, 1024, 1024);

            do
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUseProgram(textureShader);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glUniform1i(texture, 0);

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
        }
    }
}