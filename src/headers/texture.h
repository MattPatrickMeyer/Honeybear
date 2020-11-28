#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture
{
    uint32_t ID;

    uint32_t width;
    uint32_t height;

    uint32_t internal_format;
    uint32_t image_format;
    uint32_t wrap_s;
    uint32_t wrap_t;
    uint32_t filter_min;
    uint32_t filter_max;

    Texture();
    void Generate(uint32_t width, uint32_t height, unsigned char* data);
};

#endif