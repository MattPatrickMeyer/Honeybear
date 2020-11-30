#include <glad/glad.h>
#include "texture.h"

Texture::Texture() :
    width(0),
    height(0),
    internal_format(GL_RGBA),
    image_format(GL_RGBA),
    wrap_s(GL_REPEAT),
    wrap_t(GL_REPEAT)
{
    glGenTextures(1, &ID);
}

void Texture::Generate(const uint32_t width, const uint32_t height, unsigned char* data, const FilterType filter_type)
{
    this->width = width;
    this->height = height;
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    GLuint filter = GL_NEAREST;
    if(filter_type == NEAREST) filter = GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}