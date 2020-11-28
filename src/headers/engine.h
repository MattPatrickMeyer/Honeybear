#ifndef ENGINE_H
#define ENGINE_H

#include <string>

#include "texture.h"

namespace Honeybear
{
    struct Engine
    {
        void Init(int window_width, int window_height, const std::string& window_title);
        void Run();
        void Render();
        void ProcessInput();
    };
};

#endif