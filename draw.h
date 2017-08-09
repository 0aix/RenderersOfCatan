#ifndef DRAW_H
#define DRAW_H

#include <GL/glew.h>
#include "include/boardgraph.h"

namespace Catan
{
    namespace Draw
    {
        bool Initialize();
        void Uninitialize();
        void LoadAll();

        void Render(Generate::BoardGraph& graph, float length, float radius, float margin, int samples);
        void DrawTile(float cx, float cy, float length, float margin, int type);
        void DrawChit(float cx, float cy, float radius, float margin, int num);
        void DrawPort(float cx, float cy, float length, float margin, int type, int dir);

        void SaveToFile(GLuint textureID, int width, int height);
    }
}

#endif