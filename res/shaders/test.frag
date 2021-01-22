#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;

void main()
{    
    //FragColor = texture(image, TexCoords);
    FragColor = vec4(1.0);
}