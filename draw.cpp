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
        const char* const tilepaths[] =
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
        const char* const chitpaths[] =
        {
            "./chits/2.tga",
            "./chits/3.tga",
            "./chits/4.tga",
            "./chits/5.tga",
            "./chits/6.tga",
            "./chits/8.tga",
            "./chits/9.tga",
            "./chits/10.tga",
            "./chits/11.tga",
            "./chits/12.tga"
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
        const int chitnum[] = { 0, 0, 0, 1, 2, 3, 4, 0, 5, 6, 7, 8, 9 };
        GLFWwindow* window;
        GLuint tiles[8];
        GLuint chits[10];
        GLuint vertexarray;
        GLuint vertexbuffer[3];
        GLuint uvbuffer[3];
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
            glDeleteBuffers(3, vertexbuffer);
            glDeleteBuffers(3, uvbuffer);
            glDeleteTextures(8, tiles);
            glDeleteTextures(8, chits);
            glDeleteVertexArrays(1, &vertexarray);

            glDeleteProgram(diffuseShader);
            glDeleteProgram(textureShader);

            glfwTerminate();
        }

        GLuint port;

        void LoadAll()
        {
            // Load tile textures
            char* buffer = new char[2048 * 2048 * 3];
            glGenTextures(sizeof(tiles), tiles);
            for (int i = 0; i < sizeof(tiles); i++)
            {
                std::fstream fs(tilepaths[i], std::fstream::in);
                fs.seekg(18);
                fs.read(buffer, 2048 * 2048 * 3);
                fs.close();

                glBindTexture(GL_TEXTURE_2D, tiles[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }

            // Load chit textures
            glGenTextures(sizeof(chits), chits);
            for (int i = 0; i < sizeof(chits); i++)
            {
                std::fstream fs(chitpaths[i], std::fstream::in);
                fs.seekg(18);
                fs.read(buffer, 512 * 512 * 3);
                fs.close();

                glBindTexture(GL_TEXTURE_2D, chits[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            delete buffer;

            ///BELLEELHE
            buffer = new char[5120 * 6600 * 3];
            glGenTextures(1, &port);
            std::fstream fs("./ports.bmp", std::fstream::in);
            fs.seekg(18);
            fs.read(buffer, 5120 * 6600 * 3);
            fs.close();

            glBindTexture(GL_TEXTURE_2D, port);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 5120, 6600, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            delete buffer;

            // Load vertex array
            glGenVertexArrays(1, &vertexarray);
            glBindVertexArray(vertexarray);

            // Load vertices
            GLfloat vertices[] = {
                -1.0f, 1.0f,
                -0.687692824f, -0.2582173f,
                -0.476571798f, -0.444364922f,
                -0.194935694f, -0.567401952f,
                0.194935694f, -0.567401952f,
                0.476571798f, -0.444364922f,
                0.687692824f, -0.2582173f,
                1.0f, 1.0f
            };
            GLfloat uv_vertices[] = {
                3472.0f / 5120.0f, 4223.0f / 6600.0f,
                2811.0f / 5120.0f, 4045.0f / 6600.0f,
                2712.0f / 5120.0f, 3936.0f / 6600.0f,
                2650.0f / 5120.0f, 3794.0f / 6600.0f,
                2649.0f / 5120.0f, 3581.0f / 6600.0f,
                2710.0f / 5120.0f, 3444.0f / 6600.0f,
                2811.0f / 5120.0f, 3330.0f / 6600.0f,
                3468.0f / 5120.0f, 3170.0f / 6600.0f
            };
            for (int i = 0; i < 8; i++)
                uv_vertices[i * 2] += 8.0f / 5120.0f;

            // Load vertex buffers
            glGenBuffers(3, vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(preview_vertices), preview_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Load uv buffers
            glGenBuffers(3, uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_uv), tile_uv, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(preview_uv), preview_uv, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(uv_vertices), uv_vertices, GL_STATIC_DRAW);

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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendertarget[0], 0);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[0]);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[0]);

            // Multisample
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[1]);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1]);
            // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 2048, 2048, GL_TRUE);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 1024, 1024, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1], 0);
            glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[1]);
            // glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, 2048, 2048);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, 1024, 1024);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[1]);

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //BLELHEH
            glUseProgram(textureShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, port);
            glUniform1i(texture, 0);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            //DrawTile(1024.0f, 1024.0f, 400.0f, 5.0f, 0);
            //DrawChit(1024.0f, 1024.0f, 100.0f, 5.0f, 8);

            // Multisample blit + flip
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer[0]);
            //glBlitFramebuffer(0, 0, 2048, 2048, 0, 2048, 2048, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBlitFramebuffer(0, 0, 1024, 1024, 0, 1024, 1024, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            
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
            // Draw chit outline
            float rradius = radius + margin;
            glViewportIndexedf(0, cx - rradius, cy - rradius, 2.0f * rradius, 2.0f * rradius);

            glUseProgram(diffuseShader);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
            glDisableVertexAttribArray(0);

            // Draw chit
            glViewportIndexedf(0, cx - radius, cy - radius, 2.0f * radius, 2.0f * radius);

            glUseProgram(textureShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, chits[chitnum[num]]);;
            glUniform1i(texture, 0);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        void SaveToFile(GLuint textureID)
        {
            // char* buffer = new char[2048 * 2048 * 3];
            char* buffer = new char[1024 * 1024 * 3];
            glBindTexture(GL_TEXTURE_2D, textureID);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);

            char header[18] = { };
            header[2] = 2;
            // header[12] = 2048 & 0xFF;
            // header[13] = (2048 >> 8) & 0xFF;
            // header[14] = 2048 & 0xFF;
            // header[15] = (2048 >> 8) & 0xFF;
            header[12] = 1024 & 0xFF;
            header[13] = (1024 >> 8) & 0xFF;
            header[14] = 1024 & 0xFF;
            header[15] = (1024 >> 8) & 0xFF;
            header[16] = 24;

            std::fstream fs("./output/output.tga", std::fstream::out | std::fstream::trunc);
            fs.write(header, sizeof(header));
            // fs.write(buffer, 2048 * 2048 * 3);
            fs.write(buffer, 1024 * 1024 * 3);
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