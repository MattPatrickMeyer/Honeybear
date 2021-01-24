#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;

void main()
{    
    FragColor = texture(image, TexCoords) * vec4(1.0, 0.0, 0.0, 1.0);
}