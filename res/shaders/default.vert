#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec4 colour;
layout (location = 3) in float font_weight;

layout (std140) uniform Matrices
{
    mat4 projection;
};

out vec2 TexCoords;
out vec4 Colour;
out float FontWeight;

void main()
{
    TexCoords = tex_coords;
    Colour = colour;
    FontWeight = font_weight;
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}