#include "draw.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "include/boardgraph.h"
#include <thread>

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
        const char* const portpaths[] = 
        {
            "./ports/brick.tga",
            "./ports/wheat.tga",
            "./ports/sheep.tga",
            "./ports/ore.tga",
            "./ports/wood.tga",
            "./ports/321.tga"
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
        GLfloat chit_vertices[1024 * 2];
        GLfloat chit_uv[1024 * 2];
        GLfloat port_vertices[48 * 2];
        const GLfloat port_uv[] = {
            0.0f, 1-1.0f,
            0.156153588f, 1-0.37089135f,
            0.261714101f, 1-0.277817539f,
            0.402532153f, 1-0.216299024f,
            0.597467847f, 1-0.216299024f,
            0.738285899f, 1-0.277817539f,
            0.843846412f, 1-0.37089135f,
            1.0f, 1-1.0f
        };
        const int chitnum[] = { 0, 0, 0, 1, 2, 3, 4, 0, 5, 6, 7, 8, 9 };
        GLFWwindow* window;
        GLuint tiles[8];
        GLuint chits[10];
        GLuint ports[6];
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
            glDeleteTextures(10, chits);
            glDeleteTextures(6, ports);
            glDeleteVertexArrays(1, &vertexarray);

            glDeleteProgram(diffuseShader);
            glDeleteProgram(textureShader);

            glfwTerminate();
        }

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

            // Load port textures
            glGenTextures(sizeof(ports), ports);
            for (int i = 0; i < sizeof(ports); i++)
            {
                std::fstream fs(portpaths[i], std::fstream::in);
                fs.seekg(18);
                fs.read(buffer, 1024 * 1024 * 3);
                fs.close();

                glBindTexture(GL_TEXTURE_2D, ports[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            delete buffer;

            // Load vertex array
            glGenVertexArrays(1, &vertexarray);
            glBindVertexArray(vertexarray);

            // Load vertices
            // GLfloat vertices[] = {
            //     -1.0f, 1.0f,
            //     -0.687692824f, -0.2582173f,
            //     -0.476571798f, -0.444364922f,
            //     -0.194935694f, -0.567401952f,
            //     0.194935694f, -0.567401952f,
            //     0.476571798f, -0.444364922f,
            //     0.687692824f, -0.2582173f,
            //     1.0f, 1.0f
            // };

            // Load chit vertices
            float radian = 0.0f;
            for (int i = 0; i < 1024; i++)
            {
                chit_vertices[i * 2] = cos(radian);
                chit_vertices[i * 2 + 1] = sin(radian);
                radian += 2.0f * 3.141593f / 1024;
            }

            // Load port vertices
            port_vertices[0] = -0.25f; port_vertices[1] = -0.4330127f;
            port_vertices[2] = -0.171923206f; port_vertices[3] = -0.747567025f;
            port_vertices[4] = -0.1191429495f; port_vertices[5] = -0.7941039305f;
            port_vertices[6] = -0.0487339235f; port_vertices[7] = -0.824863188f;
            port_vertices[8] = 0.0487339235f; port_vertices[9] = -0.824863188f;
            port_vertices[10] = 0.1191429495f; port_vertices[11] = -0.7941039305f;
            port_vertices[12] = 0.171923206f; port_vertices[13] = -0.747567025f;
            port_vertices[14] = 0.25f; port_vertices[15] = -0.4330127f;
            for (int i = 8; i < 48; i++)
            {
                port_vertices[i * 2] = 0.5f * port_vertices[i * 2 - 16] - 0.8660254f * port_vertices[i * 2 - 15];
                port_vertices[i * 2 + 1] = 0.8660254f * port_vertices[i * 2 - 16] + 0.5f * port_vertices[i * 2 - 15];
            }

            // Load vertex buffers
            glGenBuffers(3, vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_vertices), tile_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(chit_vertices), chit_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(port_vertices), port_vertices, GL_STATIC_DRAW);

            // Load chit uv vertices
            radian = 0.0f;
            for (int i = 0; i < 1024; i++)
            {
                chit_uv[i * 2] = 0.5f * cos(radian) + 0.5f;
                chit_uv[i * 2 + 1] = 0.5f * -sin(radian) + 0.5f;
                radian += 2.0f * 3.141593f / 1024;
            }

            // Load uv buffers
            glGenBuffers(3, uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tile_uv), tile_uv, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(chit_uv), chit_uv, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(port_uv), port_uv, GL_STATIC_DRAW);

            // Load shaders
            diffuseShader = LoadShaders("./shaders/diffuse.vs", "./shaders/diffuse.fs");
            textureShader = LoadShaders("./shaders/texture.vs", "./shaders/texture.fs");
            texture = glGetUniformLocation(textureShader, "sampler");

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        }

        void Render(Generate::BoardGraph& graph, float length, float radius, float margin)
        {
            // Compute width and height
            int bwidth = graph.BoardWidth();
            int bheight = graph.BoardHeight();
            int width = ceil(bwidth * (1.5f * length + margin / 0.8660254f) + 3.5f * length + 3.0f * margin / 0.8660254f);
            int height = ceil(bheight * (1.7320508f * length + margin) + 2.0f * 1.7320508f * length + 3.0f * margin);

            if (width > 16384 || height > 16384)
            {
                printf("Board with width: %d px and height: %d px exceeds dimension limits (16384 x 16384)\n", width, height);
                return;
            }
            printf("Rendering board with width: %d px and height: %d px\n", width, height);

            // Set up regular and multisample FBOs, render targets, and depth buffers
            GLuint framebuffer[2];
            GLuint rendertarget[2];
            glGenFramebuffers(2, framebuffer);
            glGenTextures(2, rendertarget);

            // Regular
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[0]);
            glBindTexture(GL_TEXTURE_2D, rendertarget[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendertarget[0], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                printf("Failed to set up frame buffer.\n");
                glDeleteTextures(2, rendertarget);
                glDeleteFramebuffers(2, framebuffer);
                return;
            }
            
            // Multisample
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[1]);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rendertarget[1], 0);
            
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                printf("Failed to set up multisampling (image size is probably too large).\nRendered image may not be smooth.\n");
                glDeleteTextures(1, rendertarget + 1);
                glDeleteFramebuffers(1, framebuffer + 1);
                glGenFramebuffers(1, framebuffer + 1);
                glGenTextures(1, rendertarget + 1);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[1]);
                glBindTexture(GL_TEXTURE_2D, rendertarget[1]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendertarget[1], 0);
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    printf("Failed to set up second frame buffer.\n");
                    glDeleteTextures(2, rendertarget);
                    glDeleteFramebuffers(2, framebuffer);
                    return;
                }
            }

            // Render
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw water tile outline
            int rows = graph.ColumnHeight(0) + 1;
            float x = length + margin / 0.8660254f;
            float y = 1.5f * 1.7320508f * length + 2.0f * margin + (bheight - rows) * (0.8660254f * length + 0.5f * margin);
            for (int j = 0; j < rows; j++)
            {
                DrawTile(x, y, length, margin, Generate::WATER);
                y += 1.7320508f * length + margin;
            }
            x += 1.5f * length + margin / 0.8660254f;
            for (int i = 0; i < bwidth; i++)
            {
                rows = graph.ColumnHeight(i);
                float y1 = 0.5f * 1.7320508f * length + margin + (bheight - rows) * (0.8660254f * length + 0.5f * margin);
                float y2 = y1 + (rows + 1) * ((1.7320508f * length) + margin);
                DrawTile(x, y1, length, margin, Generate::WATER);
                DrawTile(x, y2, length, margin, Generate::WATER);
                x += 1.5f * length + margin / 0.8660254f;
            }
            rows = graph.ColumnHeight(bwidth - 1) + 1;
            y = 1.5f * 1.7320508f * length + 2.0f * margin + (bheight - rows) * (0.8660254f * length + 0.5f * margin);
            for (int j = 0; j < rows; j++)
            {
                DrawTile(x, y, length, margin, Generate::WATER);
                y += 1.7320508f * length + margin;
            }

            // Draw board
            auto& it = graph.ForwardIterator();

            x = 2.5f * length + 2.0f * margin / 0.8660254f;
            for (int i = 0; i < bwidth; i++)
            {
                rows = graph.ColumnHeight(i);
                y = 1.5f * 1.7320508f * length + 2.0f * margin + (bheight - rows) * (0.8660254f * length + 0.5f * margin);

                for (int j = 0; j < rows; j++)
                {
                    Generate::BoardNode* node = it.Next();
                    DrawTile(x, y, length, margin, node->type);
                    DrawChit(x, y, radius, margin, node->chit);

                    for (auto port : node->NonNullPorts())
                        DrawPort(x, y, length, margin, port.first->type, port.second);

                    y += 1.7320508f * length + margin;
                }
                x += 1.5f * length + margin / 0.8660254f;
            }

            // Blit flip
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer[0]);
            glBlitFramebuffer(0, 0, width, height, 0, height, width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            // Save texture
            SaveToFile(rendertarget[0], width, height);

            // Release resources
            glDeleteTextures(2, rendertarget);
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
            if (num >= 0)
            {
                // Draw chit outline
                float rradius = radius + margin;
                glViewportIndexedf(0, cx - rradius, cy - rradius, 2.0f * rradius, 2.0f * rradius);

                glUseProgram(diffuseShader);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
                glDisableVertexAttribArray(0);

                // Draw chit
                glViewportIndexedf(0, cx - radius, cy - radius, 2.0f * radius, 2.0f * radius);

                glUseProgram(textureShader);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, chits[chitnum[num]]);
                glUniform1i(texture, 0);

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 1024);
                glDisableVertexAttribArray(0);
                glDisableVertexAttribArray(1);
            }
        }

        void DrawPort(float cx, float cy, float length, float margin, int type, int dir)
        {
            // Draw port outline
            
            float width = 2.0f * (length + margin / 0.8660254f);

            glViewportIndexedf(0, cx - width, cy - width, 2.0f * width, 2.0f * width);
            // glViewportIndexedf(0, cx + radius * cos(theta) - width, cy + radius * sin(theta) - width, 2.0f * width, 2.0f * width);

            glUseProgram(diffuseShader);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(16 * sizeof(GLfloat) * dir));
            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glDisableVertexAttribArray(0);

            // Draw port
            width = 2.0f * (length - 2.0f * margin / 1.7320508f);
            float radius = 0.8660254f * length + margin - 0.4330127f * width;
            float theta =  -1.570796f + dir * 1.0471976f;
            glViewportIndexedf(0, cx + radius * cos(theta) - width, cy + radius * sin(theta) - width, 2.0f * width, 2.0f * width);

            glUseProgram(textureShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ports[type]);
            glUniform1i(texture, 0);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(16 * sizeof(GLfloat) * dir));
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        void SaveToFile(GLuint textureID, int width, int height)
        {
            char* buffer = new char[width * height * 3];
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);

            char header[18] = { };
            header[2] = 2;
            header[12] = width & 0xFF;
            header[13] = (width >> 8) & 0xFF;
            header[14] = height & 0xFF;
            header[15] = (height >> 8) & 0xFF;
            header[16] = 24;

            std::fstream fs("./output/board.tga", std::fstream::out | std::fstream::trunc);
            fs.write(header, sizeof(header));
            fs.write(buffer, width * height * 3);
            fs.close();
            delete buffer;

            system("open ./output/board.tga");
        }
    }
}