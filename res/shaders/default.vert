#version 330 core
layout (location = 0) in vec2 vertex; 
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec4 colour;
layout (location = 3) in float font_weight;

out vec2 TexCoords;
out vec4 Colour;
out float FontWeight;

uniform mat4 projection;

void main()
{
    TexCoords = tex_coords;
    Colour = colour;
    FontWeight = font_weight;
    gl_Position = projection * vec4(vertex, 0.0, 1.0);
}