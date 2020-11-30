#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

enum FilterType
{
    NEAREST,
    LINEAR
};

struct Texture
{
    uint32_t ID;

    uint32_t width;
    uint32_t height;

    uint32_t internal_format;
    uint32_t image_format;
    uint32_t wrap_s;
    uint32_t wrap_t;

    Texture();
    void Generate(const uint32_t width, const uint32_t height, unsigned char* data, const FilterType filter_type);
};

#endif