#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;
uniform sampler2D second_image;

void main()
{    
    FragColor = texture(second_image, TexCoords) * Colour;
}