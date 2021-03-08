#version 330 core
in vec2 TexCoords;
in vec4 Colour;
in float DistanceFactor;

out vec4 FragColor;

uniform sampler2D image;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{    
    vec3 sample = texture(image, TexCoords).rgb;
    float sigDist = DistanceFactor*(median(sample.r, sample.g, sample.b) - 0.5);
    float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
    opacity *= Colour.a;
    FragColor = vec4(Colour.rgb * opacity, opacity);
}