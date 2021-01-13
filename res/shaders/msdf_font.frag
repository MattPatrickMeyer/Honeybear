#version 330 core
in vec2 TexCoords;
in vec4 Colour;
in float PixelRange;

out vec4 FragColor;

uniform sampler2D image;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{    
    // vec2 pos = TexCoords;
    // vec3 sample = texture(image, TexCoords).rgb;
    // ivec2 sz = textureSize(image, 0).xy;
    // float dx = dFdx(pos.x) * sz.x;
    // float dy = dFdy(pos.y) * sz.y;
    // float to_pixels = 20.0 * inversesqrt(dx * dx + dy * dy);
    // float sig_dist = median(sample.r, sample.g, sample.b) - 0.5;
    // //float w = fwidth(sig_dist);
    // float o = clamp(sig_dist * to_pixels + 0.5, 0.0, 1.0);

    //float pxRange = 9.0;
    float pxRange = PixelRange;
    vec2 msdfUnit = pxRange/vec2(textureSize(image, 0));
    vec3 sample = texture(image, TexCoords).rgb;
    float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
    sigDist *= dot(msdfUnit, 0.5/fwidth(TexCoords));
    float o = clamp(sigDist + 0.5, 0.0, 1.0);

    FragColor = vec4(Colour.rgb, o * Colour.a);
}

