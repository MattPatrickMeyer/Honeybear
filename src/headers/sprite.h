#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "texture.h"

namespace Honeybear
{
    struct Sprite
    {
        std::string texture_id;
        float width;
        float height;
        float texture_x;
        float texture_y;
        float texture_w;
        float texture_h;

        void Init(const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h);
    };
};

#endif