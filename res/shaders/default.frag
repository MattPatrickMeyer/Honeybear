#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;

void main()
{    
    vec4 sample = texture(image, TexCoords);
    FragColor = sample * vec4(Colour.rgb * Colour.a, Colour.a);
}