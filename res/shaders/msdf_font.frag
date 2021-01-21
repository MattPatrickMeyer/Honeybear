#version 330 core
in vec2 TexCoords;
in vec4 Colour;
in float FontWeight;

out vec4 FragColor;

uniform sampler2D image;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{    
    // ------------------------- DON"T TOUCH
    //float px_range = 100.0;
    float px_range = 10.0;
    vec2 msdf_unit = px_range / vec2(textureSize(image, 0));
    vec3 sample = texture(image, TexCoords).rgb;
    float dist = median(sample.r, sample.g, sample.b) - 0.5;
    dist *= dot(msdf_unit, 0.5 / fwidth(TexCoords));
    float alpha = clamp(dist + 0.5, 0.0, 1.0);
    // -------------------------

    FragColor = vec4(Colour.rgb, alpha * Colour.a);
}