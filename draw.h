#ifndef DRAW_H
#define DRAW_H

#include <GL/glew.h>

namespace Catan
{
    namespace Draw
    {
        bool Initialize();
        void Uninitialize();
        void LoadAll();

        void Render();
        void DrawTile(float cx, float cy, float length, float margin, int type);
        void DrawChit(float cx, float cy, float radius, float margin, int num);

        void SaveToFile(GLuint textureID);
        void Preview(GLuint textureID);
    }
}

#endif