#ifndef DRAW_H
#define DRAW_H

namespace Catan
{
    namespace Draw
    {
        bool Initialize();
        void Uninitialize();
        void LoadAll();

        void Render();
        void DrawTile(float x, float y, float length, float margin, int type);

        void Preview();
    }
}

#endif