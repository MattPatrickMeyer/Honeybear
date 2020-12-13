#version 330 core
in vec2 TexCoords;
in vec4 Colour;
out vec4 FragColor;

uniform sampler2D image;
uniform float test_float;
uniform vec3 test_vec3;
uniform vec4 test_vec4;

void main()
{    
    FragColor = texture(image, TexCoords) * vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor = texture(image, TexCoords) * test_vec4;
    // FragColor = texture(image, TexCoords) * vec4(test_vec3, 1.0);
    //FragColor = texture(image, TexCoords) * Colour;
}