#version 330 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec4 colour;

layout (std140) uniform Matrices
{
    mat4 projection;
};

out vec2 TexCoords;
out vec4 Colour;

void main()
{
    TexCoords = tex_coords;
    Colour = colour;
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}