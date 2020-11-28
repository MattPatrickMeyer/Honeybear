#include <glad/glad.h>
#include "texture.h"

Texture::Texture() :
    width(0),
    height(0),
    internal_format(GL_RGBA),
    image_format(GL_RGBA),
    wrap_s(GL_REPEAT),
    wrap_t(GL_REPEAT),
    filter_min(GL_NEAREST),
    filter_max(GL_NEAREST)
{
    glGenTextures(1, &ID);
}

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data)
{
    this->width = width;
    this->height = height;
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
    glBindTexture(GL_TEXTURE_2D, 0);
}