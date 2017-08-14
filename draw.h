#ifndef DRAW_H
#define DRAW_H

#include <GL/glew.h>
#include "include/boardgraph.h"
#include <string>

namespace Catan
{
    namespace Draw
    {
        bool Initialize();
        void Uninitialize();
        void LoadAll();
        void LoadPortVertices(float length, float margin);

        void Render(Generate::BoardGraph& graph, float length, float radius, float margin, bool random, std::string filename);
        void DrawTile(float cx, float cy, float length, float margin, int type, bool random);
        void DrawChit(float cx, float cy, float radius, float margin, int num, bool random);
        void DrawPort(float cx, float cy, float length, float margin, int type, int dir);

        void SaveToFile(GLuint textureID, int width, int height, std::string filename);
    }
}

#endif