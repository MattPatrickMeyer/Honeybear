#include "sprite.h"
#include "graphics.h"

using namespace Honeybear;

void Sprite::Init(const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h)
{
    this->texture_id = texture_id;
    width = tex_w;
    height = tex_h;

    Texture* texture = &Graphics::textures[texture_id];
    float texture_width = texture->width;
    float texture_height = texture->height;

    texture_x = tex_x / texture_width;
    texture_y = tex_y / texture_height;
    texture_w = tex_w / texture_width;
    texture_h = tex_h / texture_height;
}