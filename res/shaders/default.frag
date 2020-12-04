#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;

void main()
{    
    //FragColor = Colour;
    //FragColor = texture(image, TexCoords);
    FragColor = texture(image, TexCoords) * Colour;
}