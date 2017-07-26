#include <stdio.h>
#include <stdlib.h>
#include "draw.h"

int main()
{
    if (Catan::Draw::Initialize())
    {
        Catan::Draw::Render();
        Catan::Draw::Uninitialize();
    }

    return 0;
}