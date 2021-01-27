#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

#include "graphics.h"
#include "engine.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

using namespace Honeybear;

std::unordered_map<std::string, uint32_t> Graphics::shaders;
std::unordered_map<std::string, Texture> Graphics::textures;
std::map<uint8_t, uint32_t> Graphics::bound_textures;
std::unordered_map<std::string, SpriteSheet*> Graphics::sprite_sheets;
std::unordered_map<uint32_t, Sprite> Graphics::sprites;
std::unordered_map<std::string, Graphics::MSDF_Font> Graphics::msdf_fonts;
std::vector<Graphics::FrameBuffer> Graphics::frame_buffers;
uint32_t Graphics::current_frame_buffer_index;
std::string Graphics::activated_shader_id;
GLFWwindow* Graphics::window;
Graphics::Batch Graphics::batch;
Graphics::ScreenRenderData Graphics::screen_render_data;
Graphics::UniformBlocks Graphics::uniform_blocks;

GLuint current_fbo = 0;
Vec4 clear_colour(0.0f, 0.0f, 0.0f, 1.0f);

std::map<uint8_t, GLenum> texture_units = 
{
    { 0,  GL_TEXTURE0  },
    { 1,  GL_TEXTURE1  },
    { 2,  GL_TEXTURE2  },
    { 3,  GL_TEXTURE3  },
    { 4,  GL_TEXTURE4  },
    { 5,  GL_TEXTURE5  },
    { 6,  GL_TEXTURE6  },
    { 7,  GL_TEXTURE7  },
    { 8,  GL_TEXTURE8  },
    { 9,  GL_TEXTURE9  },
    { 10, GL_TEXTURE10 }
};

// todo: batching does more than just quads now, so rethink the following
const int max_quad_count = 10000;
const int max_vertex_count = max_quad_count * 4;
const int max_index_count = max_quad_count * 6;

//const char* default_vert_shader = "#version 330 core\nlayout (location = 0) in vec2 vertex;\nlayout (location = 1) in vec2 tex_coords;\nlayout (location = 2) in vec4 colour;\nlayout (location = 3) in float font_weight;\nout vec2 TexCoords;\nout vec4 Colour;\nout float FontWeight;\nuniform mat4 projection;\nvoid main()\n{\nTexCoords = tex_coords;\nColour = colour;\nFontWeight = font_weight;\ngl_Position = projection * vec4(vertex, 0.0, 1.0);\n}";
const char* default_vert_shader = "#version 330 core\nlayout (location = 0) in vec2 vertex;\nlayout (location = 1) in vec2 tex_coords;\nlayout (location = 2) in vec4 colour;\nlayout (location = 3) in float font_weight;\nout vec2 TexCoords;\nout vec4 Colour;\nout float FontWeight;\nlayout (std140) uniform Matrices\n{\nmat4 projection;\n};\nvoid main()\n{\nTexCoords = tex_coords;\nColour = colour;\nFontWeight = font_weight;\ngl_Position = projection * vec4(vertex, 0.0, 1.0);\n}";
const char* default_frag_shader = "#version 330 core\nin vec2 TexCoords;\nin vec4 Colour;\nout vec4 FragColor;\nuniform sampler2D image;\nvoid main()\n{\nFragColor = texture(image, TexCoords) * Colour;\n}";

void Graphics::Init(uint32_t window_width, uint32_t window_height, const std::string& window_title)
{
    // init glfw
    glfwInit();

    // platform specific window hints
    #ifdef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    #elif __APPLE__
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    // common window hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a new window
    window = glfwCreateWindow(window_width, window_height, window_title.c_str(), NULL, NULL);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, 0);

    // center the window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width / 2 - window_width / 2, mode->height / 2 - window_height / 2);

    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, window_width, window_height);

    // enable default blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // vsync
    glfwSwapInterval(0);

    InitScreenRenderData();
    InitBatchRenderer();

    // create the default shader program
    CreateShaderProgram("default", default_vert_shader, default_frag_shader);

    InitUniformBlocks();

    // activate the default shader
    ActivateShader("default");
}

void Graphics::InitScreenRenderData()
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    screen_render_data.width = window_width;
    screen_render_data.height = window_height;

    glGenVertexArrays(1, &screen_render_data.quad_VAO);
    glBindVertexArray(screen_render_data.quad_VAO);

    glGenBuffers(1, &screen_render_data.quad_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, screen_render_data.quad_VBO);

    Vec4 colour(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex buffer[4];
    // bottom right
    buffer[0].position.x = window_width;
    buffer[0].position.y = window_height;
    buffer[0].tex_coords.x = 1.0f;
    buffer[0].tex_coords.y = 1.0f;
    buffer[0].colour = colour;
    // top right
    buffer[1].position.x = window_width;
    buffer[1].position.y = 0.0f;
    buffer[1].tex_coords.x = 1.0f;
    buffer[1].tex_coords.y = 0.0f;
    buffer[1].colour = colour;
    // top left
    buffer[2].position.x = 0.0f;
    buffer[2].position.y = 0.0f;
    buffer[2].tex_coords.x = 0.0f;
    buffer[2].tex_coords.y = 0.0f;
    buffer[2].colour = colour;
    // bottom left
    buffer[3].position.x = 0.0f;
    buffer[3].position.y = window_height;
    buffer[3].tex_coords.x = 0.0f;
    buffer[3].tex_coords.y = 1.0f;
    buffer[3].colour = colour;

    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coords));

    // colour
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenBuffers(1, &screen_render_data.quad_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_render_data.quad_IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Graphics::UpdateScreenRenderData()
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    screen_render_data.width = window_width;
    screen_render_data.height = window_height;

    Vec4 colour(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex buffer[4];
    // bottom right
    buffer[0].position.x = window_width;
    buffer[0].position.y = window_height;
    buffer[0].tex_coords.x = 1.0f;
    buffer[0].tex_coords.y = 1.0f;
    buffer[0].colour = colour;
    // top right
    buffer[1].position.x = window_width;
    buffer[1].position.y = 0.0f;
    buffer[1].tex_coords.x = 1.0f;
    buffer[1].tex_coords.y = 0.0f;
    buffer[1].colour = colour;
    // top left
    buffer[2].position.x = 0.0f;
    buffer[2].position.y = 0.0f;
    buffer[2].tex_coords.x = 0.0f;
    buffer[2].tex_coords.y = 0.0f;
    buffer[2].colour = colour;
    // bottom left
    buffer[3].position.x = 0.0f;
    buffer[3].position.y = window_height;
    buffer[3].tex_coords.x = 0.0f;
    buffer[3].tex_coords.y = 1.0f;
    buffer[3].colour = colour;

    glBindBuffer(GL_ARRAY_BUFFER, screen_render_data.quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Graphics::InitBatchRenderer()
{
    // set up batch
    batch.buffer = new Vertex[max_vertex_count];
    batch.index_buffer = new uint32_t[max_index_count];
    batch.current_index_offset = 0;
    batch.index_count = 0;

    glGenVertexArrays(1, &batch.VAO);
    glBindVertexArray(batch.VAO);

    glGenBuffers(1, &batch.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, batch.VBO);
    glBufferData(GL_ARRAY_BUFFER, max_vertex_count * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coords));

    // colour
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));

    // font weight
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, font_weight));

    // set up index element buffer
    glGenBuffers(1, &batch.IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_index_count * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

    // set up the dummy shape texture (it's just a 1x1 pixel white image)
    glGenTextures(1, &batch.shape_texture);
    glBindTexture(GL_TEXTURE_2D, batch.shape_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t colour = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour);

    // unbind everything
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    BeginBatch();
}

void Graphics::SetClearColour(const Vec4& colour)
{
    clear_colour = colour;
}

void Graphics::SetClearColour(const uint32_t frame_buffer_index, const Vec4& colour)
{
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];
    frame_buffer->clear_colour = colour;
}

void Graphics::Clear()
{
    glClearColor(clear_colour.x, clear_colour.y, clear_colour.z, clear_colour.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::ClearFrameBuffers()
{
    for(size_t i = 0; i < frame_buffers.size(); ++i)
    {
        FrameBuffer* frame_buffer = &frame_buffers[i];
        Vec4 clear_colour = frame_buffer->clear_colour;
        glClearColor(clear_colour.x, clear_colour.y, clear_colour.z, clear_colour.w);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo = 0;
}

void Graphics::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void Graphics::LoadShader(const std::string& shader_id, const char* vertex_file_name, const char* fragment_file_name)
{
    std::string vertex_code_str;
    std::string fragment_code_str;
    const char* vertex_code;
    const char* fragment_code;

    if(vertex_file_name == nullptr)
    {
        vertex_code = default_vert_shader;
    }
    else
    {
        std::ifstream vertex_shader_file(vertex_file_name);
        vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        if(vertex_shader_file.is_open())
        {
            std::stringstream vertex_shader_stream;
            vertex_shader_stream << vertex_shader_file.rdbuf();
            vertex_shader_file.close();
            vertex_code_str = vertex_shader_stream.str();
        }
        vertex_code = vertex_code_str.c_str();
    }
    
    if(fragment_file_name == nullptr)
    {
        fragment_code = default_frag_shader;
    }
    else
    {
        std::ifstream fragment_shader_file(fragment_file_name);
        fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        if(fragment_shader_file.is_open())
        {
            std::stringstream fragment_shader_stream;
            fragment_shader_stream << fragment_shader_file.rdbuf();
            fragment_shader_file.close();
            fragment_code_str = fragment_shader_stream.str();
        }
        fragment_code = fragment_code_str.c_str();
    }

    CreateShaderProgram(shader_id, vertex_code, fragment_code);
}

void Graphics::CreateShaderProgram(const std::string& shader_id, const char* vertex_code, const char* fragment_code)
{
    int success;
    char infoLog[512];

    // vertex shader compile and check
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_code, NULL);
    glCompileShader(vertex_shader_id);

    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader compile and check
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_code, NULL);
    glCompileShader(fragment_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // shader program link and check
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    shaders[shader_id] = program_id;

    // bind Matrices uniform block to this shader
    GLuint uniform_block_index = glGetUniformBlockIndex(program_id, "Matrices");
    glUniformBlockBinding(program_id, uniform_block_index, 0);
}

void Graphics::InitUniformBlocks()
{
    glGenBuffers(1, &uniform_blocks.matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_blocks.matrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 16, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniform_blocks.matrices, 0, sizeof(float) * 16);
}

void Graphics::ActivateShader(const std::string& shader_id)
{
    if(shader_id == activated_shader_id)
    {
        return;
    }

    CheckAndStartNewBatch();

    // FrameBuffer* frame_buffer = &frame_buffers[current_frame_buffer_index];
    glUseProgram(shaders[shader_id]);
    // if(frame_buffer)
    // {
    //     SetShaderProjection(shader_id, 0.0f, frame_buffer->width, 0.0f, frame_buffer->height, -1.0f, 1.0f);
    // }
    activated_shader_id = shader_id;
}

void Graphics::DeactivateShader()
{
    // when we deactivate a shader, activate the default one
    ActivateShader("default");
}

void Graphics::CheckAndStartNewBatch()
{
    if(batch.index_count > 0)
    {
        EndBatch();
        FlushBatch();
        BeginBatch();
    }
}

void Graphics::SetShaderProjection(const std::string& shader_id, const float left, const float right, const float bottom, const float top, const float z_near, const float z_far)
{
    //CheckAndStartNewBatch();

    GLfloat matrix[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    matrix[0] = 2.0f / (right - left);
    matrix[5] = 2.0f / (top - bottom);
    matrix[10] = - 2.0f / (z_far - z_near);
    matrix[12] = - (right + left) / (right - left);
    matrix[13] = - (top + bottom) / (top - bottom);
    matrix[14] = - (z_far + z_near) / (z_far - z_near);

    // uint32_t program_ID = shaders[shader_id];
    // glUniformMatrix4fv(glGetUniformLocation(program_ID, "projection"), 1, GL_FALSE, matrix);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_blocks.matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, matrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Graphics::SetShaderFloat(const std::string& shader_id, const std::string& uniform_name, const float value)
{
    CheckAndStartNewBatch();
    uint32_t program_ID = shaders[shader_id];
    glUniform1f(glGetUniformLocation(program_ID, uniform_name.c_str()), value);
}

void Graphics::SetShaderInt(const std::string& shader_id, const std::string& uniform_name, const int value)
{
    CheckAndStartNewBatch();
    uint32_t program_ID = shaders[shader_id];
    glUniform1i(glGetUniformLocation(program_ID, uniform_name.c_str()), value);
}

void Graphics::SetShaderVec2(const std::string& shader_id, const std::string& uniform_name, const Vec2& value)
{
    CheckAndStartNewBatch();
    uint32_t program_ID = shaders[shader_id];
    glUniform2f(glGetUniformLocation(program_ID, uniform_name.c_str()), value.x, value.y);
}

void Graphics::SetShaderVec3(const std::string& shader_id, const std::string& uniform_name, const Vec3& value)
{
    CheckAndStartNewBatch();
    uint32_t program_ID = shaders[shader_id];
    glUniform3f(glGetUniformLocation(program_ID, uniform_name.c_str()), value.x, value.y, value.z);
}

void Graphics::SetShaderVec4(const std::string& shader_id, const std::string& uniform_name, const Vec4& value)
{
    CheckAndStartNewBatch();
    uint32_t program_ID = shaders[shader_id];
    glUniform4f(glGetUniformLocation(program_ID, uniform_name.c_str()), value.x, value.y, value.z, value.w);
}

void Graphics::SetShaderTexture(const std::string& shader_id, const std::string& uniform_name, const GLuint texture_id, const uint8_t texture_unit)
{
    CheckAndStartNewBatch();
    BindTexture(texture_id, texture_unit);
    uint32_t program_ID = shaders[shader_id];
    glUniform1i(glGetUniformLocation(program_ID, uniform_name.c_str()), texture_unit);
}

void Graphics::SetShaderFramebufferTexture(const std::string& shader_id, const std::string& uniform_name, const uint32_t frame_buffer_index, const uint8_t texture_unit)
{
    CheckAndStartNewBatch();
    GLuint texture_id = frame_buffers[frame_buffer_index].tex_colour_buffer;
    SetShaderTexture(shader_id, uniform_name, texture_id, texture_unit);
}

void Graphics::LoadSpritesFile(const std::string& file_name, const FilterType filter_type)
{
    std::ifstream file(file_name);

    if(file.is_open())
    {
        std::string line;

        // sprite sheet name
        std::string sprite_sheet_name;
        std::getline(file, line);
        std::istringstream ssn_ss(line);
        ssn_ss >> sprite_sheet_name;

        // diffuse file name
        std::string diffuse_file_name;
        std::getline(file, line);
        std::istringstream dfn_ss(line);
        dfn_ss >> diffuse_file_name;

        // specular file name
        std::string specular_file_name;
        std::getline(file, line);
        std::istringstream sfn_ss(line);
        sfn_ss >> specular_file_name;

        // normal file name
        std::string normal_file_name;
        std::getline(file, line);
        std::istringstream nfn_ss(line);
        nfn_ss >> normal_file_name;

        SpriteSheet* sprite_sheet = LoadSpriteSheet(
            sprite_sheet_name,
            diffuse_file_name.c_str(),
            specular_file_name.c_str(),
            normal_file_name.c_str(),
            filter_type
        );

        while(std::getline(file, line))
        {
            int id, x, y, w, h;
            std::istringstream sprite_ss(line);
            sprite_ss >> id >> x >> y >> w >> h;
            CreateSprite(id, sprite_sheet, x, y, w, h);
        }
    }
}

SpriteSheet* Graphics::LoadSpriteSheet(const std::string& sprite_sheet_name, const char* diffuse, const char* specular, const char* normal, const FilterType filter_type)
{
    if(sprite_sheets.count(sprite_sheet_name) == 0)
    {
        SpriteSheet* sprite_sheet = new SpriteSheet();
        if(diffuse)
            sprite_sheet->diffuse = LoadTexture(diffuse, filter_type);
        if(specular)
            sprite_sheet->specular = LoadTexture(specular, filter_type);
        if(normal)
            sprite_sheet->normal = LoadTexture(normal, filter_type);
        sprite_sheets[sprite_sheet_name] = sprite_sheet;
    }
    return sprite_sheets[sprite_sheet_name];
}

Texture* Graphics::LoadTexture(const std::string& texture_file_name, const FilterType filter_type)
{
    int width, height, nrChannels;
    int desired_channels = 4; // 4 channels as we always want RGBA for the glTexImage2D function below
    unsigned char* data = stbi_load(texture_file_name.c_str(), &width, &height, &nrChannels, desired_channels);
    std::cout << stbi_failure_reason() << std::endl;

    GLuint filter = GL_LINEAR;
    if(filter_type == NEAREST) filter = GL_NEAREST;

    Texture* texture = &textures[texture_file_name];

    texture->width = width;
    texture->height = height;
    texture->internal_format = GL_RGBA;
    texture->image_format = GL_RGBA;
    texture->wrap_s = GL_REPEAT;
    texture->wrap_t = GL_REPEAT;

    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->internal_format, width, height, 0, texture->image_format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

void Graphics::BindTexture(const GLuint texture_id, const uint8_t texture_unit)
{
    glActiveTexture(texture_units[texture_unit]);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    bound_textures[texture_unit] = texture_id;
}

void Graphics::DrawSprite(const Sprite& sprite, const Vec2 position, const uint32_t frame_buffer_index, const Vec4& colour)
{
    DrawSprite(sprite, position, Vec2(sprite.width, sprite.height), frame_buffer_index, DIFFUSE, colour);
}

void Graphics::DrawSprite(const Sprite& sprite, const Vec2 position, const Vec2 size, const uint32_t frame_buffer_index, const Vec4& colour)
{
    DrawSprite(sprite, position, size, frame_buffer_index, DIFFUSE, colour);
}

void Graphics::DrawSprite(const Sprite& sprite, const Vec2 position, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour)
{
    DrawSprite(sprite, position, Vec2(sprite.width, sprite.height), frame_buffer_index, sprite_sheet_layer, colour);
}

void Graphics::DrawSprite(const Sprite& sprite, const Vec2 position, const Vec2 size, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour)
{
    int indices_count = 6;
    uint32_t texture_id = sprite.sprite_sheet->diffuse->ID;

    switch(sprite_sheet_layer)
    {
        case DIFFUSE :
        {
            texture_id = sprite.sprite_sheet->diffuse->ID;
            break;
        }
        case SPECULAR :
        {
            texture_id = sprite.sprite_sheet->specular->ID;
            break;
        }
        case NORMAL :
        {
            texture_id = sprite.sprite_sheet->normal->ID;
            break;
        }
    }

    DoBatchRenderSetUp(frame_buffer_index, texture_id, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // bottom right
    batch.buffer_ptr->position.x = (position.x + size.x) * pixel_size;
    batch.buffer_ptr->position.y = (position.y + size.y) * pixel_size;
    batch.buffer_ptr->tex_coords.x = sprite.texture_x + sprite.texture_w;
    batch.buffer_ptr->tex_coords.y = sprite.texture_y + sprite.texture_h;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top right
    batch.buffer_ptr->position.x = (position.x + size.x) * pixel_size;
    batch.buffer_ptr->position.y = (position.y) * pixel_size;
    batch.buffer_ptr->tex_coords.x = sprite.texture_x + sprite.texture_w;
    batch.buffer_ptr->tex_coords.y = sprite.texture_y;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top left
    batch.buffer_ptr->position.x = (position.x) * pixel_size;
    batch.buffer_ptr->position.y = (position.y) * pixel_size;
    batch.buffer_ptr->tex_coords.x = sprite.texture_x;
    batch.buffer_ptr->tex_coords.y = sprite.texture_y;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // bottom left
    batch.buffer_ptr->position.x = (position.x) * pixel_size;
    batch.buffer_ptr->position.y = (position.y + size.y) * pixel_size;
    batch.buffer_ptr->tex_coords.x = sprite.texture_x;
    batch.buffer_ptr->tex_coords.y = sprite.texture_y + sprite.texture_h;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // first tri indices
    batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 2] = 3 + batch.current_index_offset;

    // second tri indices
    batch.index_buffer[batch.index_count + 3] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 4] = 2 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 5] = 3 + batch.current_index_offset;

    batch.current_index_offset += 4;
    batch.index_count += indices_count;
}

void Graphics::FillTriangle(const Vec2& pos_a, const Vec2& pos_b, const Vec2& pos_c, const uint32_t frame_buffer_index, const Vec4& colour)
{
    int indices_count = 3;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    batch.buffer_ptr->position.x = pos_a.x * pixel_size;
    batch.buffer_ptr->position.y = pos_a.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    batch.buffer_ptr->position.x = pos_b.x * pixel_size;
    batch.buffer_ptr->position.y = pos_b.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    batch.buffer_ptr->position.x = pos_c.x * pixel_size;
    batch.buffer_ptr->position.y = pos_c.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 2] = 2 + batch.current_index_offset;

    batch.current_index_offset += 3;
    batch.index_count += indices_count;
}

void Graphics::FillRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour)
{
    int indices_count = 6;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // bottom right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = (y + h) * pixel_size;
    batch.buffer_ptr->tex_coords.x = 1.0f;
    batch.buffer_ptr->tex_coords.y = 1.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 1.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top left
    batch.buffer_ptr->position.x = x * pixel_size;
    batch.buffer_ptr->position.y = y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // bottom left
    batch.buffer_ptr->position.x = x * pixel_size;
    batch.buffer_ptr->position.y = (y + h) * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 1.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // first tri indices
    batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 2] = 3 + batch.current_index_offset;

    // second tri indices
    batch.index_buffer[batch.index_count + 3] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 4] = 2 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 5] = 3 + batch.current_index_offset;

    batch.current_index_offset += 4;
    batch.index_count += indices_count;
}

void Graphics::FillCircle(const Vec2& pos, const float radius, const uint32_t frame_buffer_index, const Vec4& colour)
{
    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // -----------------------------
    // the below formula was taken from here: https://stackoverflow.com/questions/11774038/how-to-render-a-circle-with-as-few-vertices-as-possible
    float error = 0.25f;
    float th = std::acos(2 * ((1 - error / (radius * pixel_size)) * (1 - error / (radius * pixel_size))) - 1);
    int number_of_sides = std::ceil(2 * PI / th);
    // -----------------------------

    float angle_per_side = 360.0f / number_of_sides;

    int indices_count = number_of_sides * 3;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count);

    // center
    batch.buffer_ptr->position.x = pos.x * pixel_size;
    batch.buffer_ptr->position.y = pos.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // all the others
    for(size_t i = 0; i < number_of_sides; ++i)
    {
        float degrees = i * angle_per_side;

        float radians = degrees * (PI / 180.0f);

        batch.buffer_ptr->position.x = (pos.x + std::cos(radians) * radius) * pixel_size;
        batch.buffer_ptr->position.y = (pos.y + std::sin(radians) * radius) * pixel_size;
        batch.buffer_ptr->tex_coords.x = 0.0f;
        batch.buffer_ptr->tex_coords.y = 0.0f;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr++;

        batch.index_buffer[batch.index_count + 0] = batch.current_index_offset + 0;
        batch.index_buffer[batch.index_count + 1] = batch.current_index_offset + 1 * i;
        batch.index_buffer[batch.index_count + 2] = batch.current_index_offset + 1 * (i + 1);

        batch.index_count += 3;
    }

    batch.index_buffer[batch.index_count + 0] = batch.current_index_offset + 0;
    batch.index_buffer[batch.index_count + 1] = batch.current_index_offset + number_of_sides;
    batch.index_buffer[batch.index_count + 2] = batch.current_index_offset + 1;

    batch.index_count += 3;

    batch.current_index_offset += number_of_sides + 1;
}

void Graphics::FillConvexPoly(const std::vector<Vec2>& points, const uint32_t frame_buffer_index, const Vec4& colour)
{
    int tri_count = points.size() - 2;
    int indices_count = tri_count * 3;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // set up vertices
    for(size_t i = 0; i < points.size(); ++i)
    {
        batch.buffer_ptr->position.x = points[i].x * pixel_size;
        batch.buffer_ptr->position.y = points[i].y * pixel_size;
        batch.buffer_ptr->tex_coords.x = 0.0f;
        batch.buffer_ptr->tex_coords.y = 0.0f;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr++;
    }

    // set up indices
    for(size_t i = 0; i < tri_count; ++i)
    {
        batch.index_buffer[batch.index_count + 0] = batch.current_index_offset + 0;
        batch.index_buffer[batch.index_count + 1] = batch.current_index_offset + i + 1;
        batch.index_buffer[batch.index_count + 2] = batch.current_index_offset + i + 2;

        batch.index_count += 3;
    }

    batch.current_index_offset += points.size();
}

void Graphics::DrawLine(const Vec2& start, const Vec2& end, const uint32_t frame_buffer_index, const Vec4& colour)
{
    int indices_count = 2;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count, true);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // start
    batch.buffer_ptr->position.x = start.x * pixel_size;
    batch.buffer_ptr->position.y = start.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // end
    batch.buffer_ptr->position.x = end.x * pixel_size;
    batch.buffer_ptr->position.y = end.y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // indices
    batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;

    batch.index_count += indices_count;
    batch.current_index_offset += indices_count;
}

void Graphics::DrawRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour)
{
    // todo: slow (but who cares?) this shouldn't use DrawLine and should be done properly with GL_LINE_STRIP
    DrawLine(Vec2(x, y), Vec2(x + w, y), frame_buffer_index, colour);
    DrawLine(Vec2(x + w, y), Vec2(x + w, y + h), frame_buffer_index, colour);
    DrawLine(Vec2(x + w, y + h), Vec2(x, y + h), frame_buffer_index, colour);
    DrawLine(Vec2(x, y + h), Vec2(x, y), frame_buffer_index, colour);
}

void Graphics::DrawCircle(const Vec2& pos, const float radius, const uint32_t frame_buffer_index, const Vec4& colour)
{
    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // -----------------------------
    // the below formula was taken from here: https://stackoverflow.com/questions/11774038/how-to-render-a-circle-with-as-few-vertices-as-possible
    float error = 0.25f;
    float th = std::acos(2 * ((1 - error / (radius * pixel_size)) * (1 - error / (radius * pixel_size))) - 1);
    int number_of_sides = std::ceil(2 * PI / th);
    // -----------------------------

    float angle_per_side = 360.0f / number_of_sides;

    // todo: dumb and slow?
    for(size_t i = 0; i < number_of_sides; ++i)
    {
        float degrees = i * angle_per_side;
        float radians = degrees * (PI / 180.0f);
        Vec2 start(
            pos.x + std::cos(radians) * radius,
            pos.y + std::sin(radians) * radius
        );
        radians = (degrees + angle_per_side) * (PI / 180.0f);
        Vec2 end(
            pos.x + std::cos(radians) * radius,
            pos.y + std::sin(radians) * radius
        );
        DrawLine(start, end, frame_buffer_index, colour);
    }
}

void Graphics::DrawPoly(const std::vector<Vec2>& points, const uint32_t frame_buffer_index, const Vec4& colour)
{
    for(size_t i = 0; i < points.size(); ++i)
    {
        size_t next_i = (i + 1) % points.size();
        DrawLine(points[i], points[next_i], frame_buffer_index, colour);
    }
}

void Graphics::BeginBatch()
{
    batch.buffer_ptr = batch.buffer;
}

void Graphics::EndBatch()
{
    if(batch.index_count == 0) return;
    GLsizeiptr size = (uint8_t*)batch.buffer_ptr - (uint8_t*)batch.buffer;
    glBindBuffer(GL_ARRAY_BUFFER, batch.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch.buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.IB);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch.index_count * sizeof(uint32_t), batch.index_buffer);
}

void Graphics::FlushBatch()
{
    if(batch.index_count == 0) return;
    glBindVertexArray(batch.VAO);
    glDrawElements(batch.draw_lines ? GL_LINES : GL_TRIANGLES, batch.index_count, GL_UNSIGNED_INT, nullptr);
    batch.index_count = 0;
    batch.current_index_offset = 0;
    glBindVertexArray(0);

    // todo: probably not the best (fastest) place to put this
    // - the frame buffer only needs to be blitted if it's actually being used/rendered
    // - should probably put it in renderframebuffer or similar
    FrameBuffer* current_frame_buffer = &frame_buffers[current_frame_buffer_index];
    if(current_frame_buffer->multisampled)
    {
        std::string current_shader = activated_shader_id;
        ActivateShader("default");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, current_frame_buffer->FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, current_frame_buffer->intermediate_FBO);
        glBlitFramebuffer(0, 0, current_frame_buffer->width, current_frame_buffer->height, 0, 0, current_frame_buffer->width, current_frame_buffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, current_frame_buffer->FBO);

        ActivateShader(current_shader);
    }
}

void Graphics::DoBatchRenderSetUp(const uint32_t frame_buffer_index, const GLuint tex_id, const uint32_t num_indices, bool draw_lines)
{
    //if(frame_buffer_index != current_frame_buffer_index)
    if(frame_buffers[frame_buffer_index].FBO != current_fbo)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    bool should_start_new_batch = false;
    bool should_bind_texture = false;

    //uint32_t texture_id = batch.shape_texture;
    if(bound_textures[0] != tex_id)
    {
        should_start_new_batch = true;
        should_bind_texture = true;
    }

    if(batch.index_count + num_indices > max_index_count)
    {
        should_start_new_batch = true;
    }

    if(batch.draw_lines != draw_lines)
    {
        should_start_new_batch = true;
    }

    if(should_start_new_batch)
    {
        CheckAndStartNewBatch();
    }

    if(should_bind_texture)
    {
        BindTexture(tex_id, 0);
    }

    batch.draw_lines = draw_lines;
}

Sprite* Graphics::GetSprite(const uint32_t sprite_id)
{
    if(sprites.count(sprite_id) > 0)
    {
        return &sprites[sprite_id];
    }
    return nullptr;
}

void Graphics::CreateSprite(const uint32_t sprite_id, SpriteSheet* sprite_sheet, int tex_x, int tex_y, int tex_w, int tex_h)
{
    sprites[sprite_id].sprite_sheet = sprite_sheet;
    sprites[sprite_id].width = tex_w;
    sprites[sprite_id].height = tex_h;

    Texture* texture = sprite_sheet->diffuse;
    float texture_width = texture->width;
    float texture_height = texture->height;

    sprites[sprite_id].texture_x = tex_x / texture_width;
    sprites[sprite_id].texture_y = tex_y / texture_height;
    sprites[sprite_id].texture_w = tex_w / texture_width;
    sprites[sprite_id].texture_h = tex_h / texture_height;
}

void Graphics::BindFrameBuffer(const uint32_t frame_buffer_index)
{
    // flush any batched quads to the previous frame buffer
    CheckAndStartNewBatch();

    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
    current_fbo = frame_buffer->FBO;
    glViewport(0, 0, frame_buffer->width, frame_buffer->height);
    current_frame_buffer_index = frame_buffer_index;

    // make sure the shader projection matrix is set up
    SetShaderProjection(activated_shader_id, 0.0f, frame_buffer->width, 0.0f, frame_buffer->height, -1.0f, 1.0f);
}

uint32_t Graphics::AddMultiSampledFrameBuffer(const uint32_t samples)
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    return AddMultiSampledFrameBuffer(window_width, window_height, samples, true, true);
}

uint32_t Graphics::AddMultiSampledFrameBuffer(const uint32_t width, const uint32_t height, const uint32_t samples, const bool use_game_pixel_scaling, const bool mapped_to_window_resolution)
{
    // todo: cleanup: mostly copy paste from AddFrameBuffer
    frame_buffers.push_back(FrameBuffer());
    uint32_t frame_buffer_index = frame_buffers.size() - 1;
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    // ---------------------------------------------------
    glGenFramebuffers(1, &frame_buffer->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);

    glGenTextures(1, &frame_buffer->tex_colour_buffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frame_buffer->tex_colour_buffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frame_buffer->tex_colour_buffer, 0);
    // ---------------------------------------------------

    // ---------------------------------------------------
    glGenFramebuffers(1, &frame_buffer->intermediate_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->intermediate_FBO);

    glGenTextures(1, &frame_buffer->intermediate_tex_colour_buffer);
    glBindTexture(GL_TEXTURE_2D, frame_buffer->intermediate_tex_colour_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->intermediate_tex_colour_buffer, 0);
    // ---------------------------------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frame_buffer->width = width;
    frame_buffer->height = height;
    frame_buffer->game_pixel_size = use_game_pixel_scaling ? width / Honeybear::game_width : 1.0f;
    frame_buffer->use_game_pixel_scaling = use_game_pixel_scaling;
    frame_buffer->mapped_to_window_resolution = mapped_to_window_resolution;
    frame_buffer->multisampled = true;
    frame_buffer->samples = samples;
    frame_buffer->clear_colour = Vec4(1.0f, 1.0f, 1.0f, 0.0f);

    // ----------------------------------------------------------------------------
    // set up the VAO used to for rendering another framebuffer to this framebuffer
    // ----------------------------------------------------------------------------
    glGenVertexArrays(1, &frame_buffer->quad_VAO);
    glBindVertexArray(frame_buffer->quad_VAO);

    glGenBuffers(1, &frame_buffer->quad_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, frame_buffer->quad_VBO);

    Vec4 colour(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex buffer[4];
    // bottom right
    buffer[0].position.x = width;
    buffer[0].position.y = height;
    buffer[0].tex_coords.x = 1.0f;
    buffer[0].tex_coords.y = 1.0f;
    buffer[0].colour = colour;
    // top right
    buffer[1].position.x = width;
    buffer[1].position.y = 0.0f;
    buffer[1].tex_coords.x = 1.0f;
    buffer[1].tex_coords.y = 0.0f;
    buffer[1].colour = colour;
    // top left
    buffer[2].position.x = 0.0f;
    buffer[2].position.y = 0.0f;
    buffer[2].tex_coords.x = 0.0f;
    buffer[2].tex_coords.y = 0.0f;
    buffer[2].colour = colour;
    // bottom left
    buffer[3].position.x = 0.0f;
    buffer[3].position.y = height;
    buffer[3].tex_coords.x = 0.0f;
    buffer[3].tex_coords.y = 1.0f;
    buffer[3].colour = colour;

    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coords));

    // colour
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenBuffers(1, &frame_buffer->quad_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frame_buffer->quad_IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------

    // if this is the first frame buffer, bind it?
    if(frame_buffer_index == 0)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    return frame_buffer_index;
}

uint32_t Graphics::AddFrameBuffer()
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    return AddFrameBuffer(window_width, window_height, true, true);
}

uint32_t Graphics::AddFrameBuffer(const uint32_t width, const uint32_t height, const bool use_game_pixel_scaling, const bool mapped_to_window_resolution)
{
    frame_buffers.push_back(FrameBuffer());
    uint32_t frame_buffer_index = frame_buffers.size() - 1;
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    glGenFramebuffers(1, &frame_buffer->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);

    glGenTextures(1, &frame_buffer->tex_colour_buffer);
    glBindTexture(GL_TEXTURE_2D, frame_buffer->tex_colour_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->tex_colour_buffer, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frame_buffer->width = width;
    frame_buffer->height = height;
    frame_buffer->game_pixel_size = use_game_pixel_scaling ? width / Honeybear::game_width : 1.0f; // todo: janky as fuck
    frame_buffer->use_game_pixel_scaling = use_game_pixel_scaling;
    frame_buffer->mapped_to_window_resolution = mapped_to_window_resolution;
    frame_buffer->multisampled = false;
    frame_buffer->clear_colour = Vec4(1.0f, 1.0f, 1.0f, 0.0f);

    // ----------------------------------------------------------------------------
    // set up the VAO used to for rendering another framebuffer to this framebuffer
    // ----------------------------------------------------------------------------
    glGenVertexArrays(1, &frame_buffer->quad_VAO);
    glBindVertexArray(frame_buffer->quad_VAO);

    glGenBuffers(1, &frame_buffer->quad_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, frame_buffer->quad_VBO);

    Vec4 colour(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex buffer[4];
    // bottom right
    buffer[0].position.x = width;
    buffer[0].position.y = height;
    buffer[0].tex_coords.x = 1.0f;
    buffer[0].tex_coords.y = 1.0f;
    buffer[0].colour = colour;
    // top right
    buffer[1].position.x = width;
    buffer[1].position.y = 0.0f;
    buffer[1].tex_coords.x = 1.0f;
    buffer[1].tex_coords.y = 0.0f;
    buffer[1].colour = colour;
    // top left
    buffer[2].position.x = 0.0f;
    buffer[2].position.y = 0.0f;
    buffer[2].tex_coords.x = 0.0f;
    buffer[2].tex_coords.y = 0.0f;
    buffer[2].colour = colour;
    // bottom left
    buffer[3].position.x = 0.0f;
    buffer[3].position.y = height;
    buffer[3].tex_coords.x = 0.0f;
    buffer[3].tex_coords.y = 1.0f;
    buffer[3].colour = colour;

    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coords));

    // colour
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenBuffers(1, &frame_buffer->quad_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frame_buffer->quad_IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------

    // if this is the first frame buffer, bind it?
    if(frame_buffer_index == 0)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    return frame_buffer_index;
}

void Graphics::UpdateFrameBufferSize(const uint32_t frame_buffer_index, const uint32_t width, const uint32_t height)
{
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    // todo: cleanup
    if(frame_buffer->multisampled)
    {
        glDeleteTextures(1, &frame_buffer->tex_colour_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
        glGenTextures(1, &frame_buffer->tex_colour_buffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frame_buffer->tex_colour_buffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, frame_buffer->samples, GL_RGBA, width, height, GL_TRUE); // todo: hardcoded samples (8)
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frame_buffer->tex_colour_buffer, 0);

        glDeleteTextures(1, &frame_buffer->intermediate_tex_colour_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->intermediate_FBO);
        glGenTextures(1, &frame_buffer->intermediate_tex_colour_buffer);
        glBindTexture(GL_TEXTURE_2D, frame_buffer->intermediate_tex_colour_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->intermediate_tex_colour_buffer, 0);
    }
    else
    {
        // delete the existing FBO texture/s
        glDeleteTextures(1, &frame_buffer->tex_colour_buffer);

        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
        glGenTextures(1, &frame_buffer->tex_colour_buffer);
        glBindTexture(GL_TEXTURE_2D, frame_buffer->tex_colour_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->tex_colour_buffer, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo = 0;

    frame_buffer->width = width;
    frame_buffer->height = height;
    if(frame_buffer->use_game_pixel_scaling)
    {
        frame_buffer->game_pixel_size = width / Honeybear::game_width;
    }

    // ----------------------------------------------------------------------------
    // update the VAO used to for rendering another framebuffer to this framebuffer
    // ----------------------------------------------------------------------------
    Vec4 colour(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex buffer[4];
    // bottom right
    buffer[0].position.x = width;
    buffer[0].position.y = height;
    buffer[0].tex_coords.x = 1.0f;
    buffer[0].tex_coords.y = 1.0f;
    buffer[0].colour = colour;
    // top right
    buffer[1].position.x = width;
    buffer[1].position.y = 0.0f;
    buffer[1].tex_coords.x = 1.0f;
    buffer[1].tex_coords.y = 0.0f;
    buffer[1].colour = colour;
    // top left
    buffer[2].position.x = 0.0f;
    buffer[2].position.y = 0.0f;
    buffer[2].tex_coords.x = 0.0f;
    buffer[2].tex_coords.y = 0.0f;
    buffer[2].colour = colour;
    // bottom left
    buffer[3].position.x = 0.0f;
    buffer[3].position.y = height;
    buffer[3].tex_coords.x = 0.0f;
    buffer[3].tex_coords.y = 1.0f;
    buffer[3].colour = colour;

    glBindBuffer(GL_ARRAY_BUFFER, frame_buffer->quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // ----------------------------------------------------------------------------
}

void Graphics::RenderFrameBuffer(const uint32_t frame_buffer_index)
{
    RenderFrameBuffer(frame_buffer_index, Vec2(0.0f));
}

void Graphics::RenderFrameBuffer(const uint32_t frame_buffer_index, const Vec2& offset)
{
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    // before we render the frame buffer to the screen, make sure all batched quads have been flushed to their buffer
    CheckAndStartNewBatch();

    GLuint source_colour_buffer = frame_buffer->multisampled
        ? frame_buffer->intermediate_tex_colour_buffer
        : frame_buffer->tex_colour_buffer;

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    // frame buffer textures are upside down, so use a projection that will flip them the right way
    SetShaderProjection(activated_shader_id, 0.0f - offset.x, (float)window_width - offset.x, (float)window_height - offset.y, 0.0f - offset.y, -1.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    current_fbo = 0;
    BindTexture(source_colour_buffer, 0);
    glBindVertexArray(screen_render_data.quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Graphics::RenderFrameBufferToFrameBuffer(const uint32_t source_frame_buffer_index, const uint32_t dest_frame_buffer_index)
{
    FrameBuffer* source_frame_buffer = &frame_buffers[source_frame_buffer_index];
    FrameBuffer* dest_frame_buffer = &frame_buffers[dest_frame_buffer_index];

    CheckAndStartNewBatch();

    GLuint source_colour_buffer = source_frame_buffer->multisampled
        ? source_frame_buffer->intermediate_tex_colour_buffer
        : source_frame_buffer->tex_colour_buffer;

    BindFrameBuffer(dest_frame_buffer_index);
    BindTexture(source_colour_buffer, 0);
    glBindVertexArray(dest_frame_buffer->quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Graphics::RenderFrameBufferToQuad(const uint32_t source_frame_buffer_index, const float x, const float y, const float w, const float h, const uint32_t dest_frame_buffer_index, const Vec4& colour)
{
    int indices_count = 6;

    FrameBuffer* src = &frame_buffers[source_frame_buffer_index];
    GLuint tex_buffer = src->multisampled ? src->intermediate_tex_colour_buffer : src->tex_colour_buffer;

    DoBatchRenderSetUp(dest_frame_buffer_index, tex_buffer, indices_count);

    float pixel_size = frame_buffers[dest_frame_buffer_index].game_pixel_size;

    // bottom right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = (y + h) * pixel_size;
    batch.buffer_ptr->tex_coords.x = 1.0f;
    batch.buffer_ptr->tex_coords.y = 1.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 1.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top left
    batch.buffer_ptr->position.x = x * pixel_size;
    batch.buffer_ptr->position.y = y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // bottom left
    batch.buffer_ptr->position.x = x * pixel_size;
    batch.buffer_ptr->position.y = (y + h) * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 1.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // first tri indices
    batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 2] = 3 + batch.current_index_offset;

    // second tri indices
    batch.index_buffer[batch.index_count + 3] = 1 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 4] = 2 + batch.current_index_offset;
    batch.index_buffer[batch.index_count + 5] = 3 + batch.current_index_offset;

    batch.current_index_offset += 4;
    batch.index_count += indices_count;
}

void Graphics::ChangeResolution(const uint32_t width, const uint32_t height)
{
    // change the window size
    glfwSetWindowSize(window, width, height);

    // center the window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);

    glViewport(0, 0, width, height);

    // update every framebuffer that is mapped to size of the window
    for(size_t i = 0; i < frame_buffers.size(); ++i)
    {
        if(frame_buffers[i].mapped_to_window_resolution)
        {
            UpdateFrameBufferSize(i, width, height);
        }
    }

    // update the quad VAO that is used for rendering framebuffers to the screen
    UpdateScreenRenderData();
}

void Graphics::GetResolution(int* width, int* height)
{
    glfwGetWindowSize(window, width, height);
}

void Graphics::ToggleFullscreen(const bool enabled)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(window, enabled ? monitor : nullptr, 0, 0, screen_render_data.width, screen_render_data.height, mode->refreshRate);

    // if(!enabled)
    // {
    //     glfwSetWindowPos(window, mode->width / 2 - screen_render_data.width / 2, mode->height / 2 - screen_render_data.height / 2);
    // }
}

void Graphics::ToggleVSync(const bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
}

Graphics::MSDF_Font* Graphics::LoadMSDFFont(const std::string& font_id, const std::string& font_atlas_file_name, const std::string& font_data_file_name)
{
    MSDF_Font* font = &msdf_fonts[font_id];

    font->tallest_char_height = 0.0f;
    font->texture = LoadTexture(font_atlas_file_name, LINEAR);

    std::ifstream file(font_data_file_name);

    if(file.is_open())
    {
        std::string line;

        while(std::getline(file, line))
        {
            MSDF_CharData data;

            std::istringstream char_data_ss(line);
            std::vector<std::string> fields;
            fields.reserve(10);
            std::string field;
            while(std::getline(char_data_ss, field, ','))
            {
                fields.push_back(field);
            }

            data.unicode =             std::stoi(fields[0]);
            data.advance =             std::stof(fields[1]);
            data.plane_bounds.left =   std::stof(fields[2]);
            data.plane_bounds.bottom = 1.0f - std::stof(fields[3]);
            data.plane_bounds.right =  std::stof(fields[4]);
            data.plane_bounds.top =    1.0f - std::stof(fields[5]);
            data.atlas_bounds.left =   std::stof(fields[6]);
            data.atlas_bounds.bottom = font->texture->height - std::stof(fields[7]);
            data.atlas_bounds.right =  std::stof(fields[8]);
            data.atlas_bounds.top =    font->texture->height - std::stof(fields[9]);

            font->data[data.unicode] = data;

            float height = data.atlas_bounds.bottom - data.atlas_bounds.top;

            if(height > font->tallest_char_height)
            {
                font->tallest_char_height = height;
            }
        }
    }

    return font;
}

void Graphics::RenderText(const std::string& text, const Vec2& position, const std::string& font_id, const float size, const uint32_t frame_buffer_index, const Vec4& colour)
{
    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;
    float adjusted_size = size * pixel_size;

    float font_weight = 0.5f;

    // todo: dumb
    if(adjusted_size <= 30.0f)
    {
        font_weight = 1.0f;
    }

    MSDF_Font* font = &msdf_fonts[font_id];
    float atlas_width = font->texture->width;
    float atlas_height = font->texture->height;

    size_t str_len = text.length();
    const char* text_c_str = text.c_str();

    //float size_mod = size / font->tallest_char_height;

    float cursor_x = position.x * pixel_size;
    float cursor_y = position.y * pixel_size;

    for(size_t i = 0; i < str_len; ++i)
    {
        int indices_count = 6;

        DoBatchRenderSetUp(frame_buffer_index, font->texture->ID, indices_count);

        char c = text_c_str[i];
        int ascii_code = static_cast<int>(c);

        // handle spaces
        if(ascii_code == 32)
        {
            // don't render anything for a space, just move the cursor
            MSDF_CharData char_data = font->data[ascii_code];
            cursor_x += char_data.advance * adjusted_size;
            continue;
        }

        MSDF_CharData char_data = font->data[ascii_code];

        // todo: just calculate these at import??
        float tex_left = char_data.atlas_bounds.left / atlas_width;
        float tex_top = char_data.atlas_bounds.top / atlas_height;
        float tex_right = char_data.atlas_bounds.right / atlas_width;
        float tex_bottom = char_data.atlas_bounds.bottom / atlas_height;

        float quad_left = cursor_x + (char_data.plane_bounds.left * adjusted_size);
        float quad_top = cursor_y + (char_data.plane_bounds.top * adjusted_size);
        float quad_right = cursor_x + (char_data.plane_bounds.right * adjusted_size);
        float quad_bottom = cursor_y + (char_data.plane_bounds.bottom * adjusted_size);

        // bottom right
        batch.buffer_ptr->position.x = quad_right;
        batch.buffer_ptr->position.y = quad_bottom;
        batch.buffer_ptr->tex_coords.x = tex_right;
        batch.buffer_ptr->tex_coords.y = tex_bottom;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr->font_weight = font_weight;
        batch.buffer_ptr++;

        // top right
        batch.buffer_ptr->position.x = quad_right;
        batch.buffer_ptr->position.y = quad_top;
        batch.buffer_ptr->tex_coords.x = tex_right;
        batch.buffer_ptr->tex_coords.y = tex_top;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr->font_weight = font_weight;
        batch.buffer_ptr++;

        // top left
        batch.buffer_ptr->position.x = quad_left;
        batch.buffer_ptr->position.y = quad_top;
        batch.buffer_ptr->tex_coords.x = tex_left;
        batch.buffer_ptr->tex_coords.y = tex_top;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr->font_weight = font_weight;
        batch.buffer_ptr++;

        // bottom left
        batch.buffer_ptr->position.x = quad_left;
        batch.buffer_ptr->position.y = quad_bottom;
        batch.buffer_ptr->tex_coords.x = tex_left;
        batch.buffer_ptr->tex_coords.y = tex_bottom;
        batch.buffer_ptr->colour = colour;
        batch.buffer_ptr->font_weight = font_weight;
        batch.buffer_ptr++;

        // first tri indices
        batch.index_buffer[batch.index_count + 0] = 0 + batch.current_index_offset;
        batch.index_buffer[batch.index_count + 1] = 1 + batch.current_index_offset;
        batch.index_buffer[batch.index_count + 2] = 3 + batch.current_index_offset;

        // second tri indices
        batch.index_buffer[batch.index_count + 3] = 1 + batch.current_index_offset;
        batch.index_buffer[batch.index_count + 4] = 2 + batch.current_index_offset;
        batch.index_buffer[batch.index_count + 5] = 3 + batch.current_index_offset;

        batch.current_index_offset += 4;
        batch.index_count += indices_count;

        //x += char_data.advance * size * size_mod;
        cursor_x += char_data.advance * adjusted_size;
        // x += font_data.width;
    }
}

void Graphics::CalcTextDimensions(const std::string& text, const std::string& font_id, const float size, float* width, float* height)
{
    float min_x = 0.0f;
    float max_x = 0.0f;
    float min_y = 0.0f;
    float max_y = 0.0f;

    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    MSDF_Font* font = &msdf_fonts[font_id];
    size_t str_len = text.length();
    const char* text_c_str = text.c_str();

    for(size_t i = 0; i < str_len; ++i)
    {
        char c = text_c_str[i];
        int ascii_code = static_cast<int>(c);
        MSDF_CharData char_data = font->data[ascii_code];

        float quad_left = cursor_x + (char_data.plane_bounds.left * size);
        float quad_top = cursor_y + (char_data.plane_bounds.top * size);
        float quad_right = cursor_x + (char_data.plane_bounds.right * size);
        float quad_bottom = cursor_y + (char_data.plane_bounds.bottom * size);

        if(quad_left < min_x)    min_x = quad_left;
        if(quad_right > max_x)   max_x = quad_right;
        if(quad_top < min_y)     min_y = quad_top;
        if(quad_bottom > max_y)  max_y = quad_bottom;

        cursor_x += char_data.advance * size;
    }

    // todo: temp hack
    max_x = cursor_x;

    *width = max_x - min_x;
    *height = max_y - min_y;
}

void Graphics::EnableBlending()
{
    CheckAndStartNewBatch();
    glEnable(GL_BLEND);
}

void Graphics::DisableBlending()
{
    CheckAndStartNewBatch();
    glDisable(GL_BLEND);
}

void Graphics::SetBlendFunction(GLenum source_factor, GLenum dest_factor)
{
    CheckAndStartNewBatch();
    glBlendFunc(source_factor, dest_factor);
}

void Graphics::SetBlendFunctionSeperate(GLenum source_factor_rgb, GLenum dest_factor_rgb, GLenum source_factor_alpha, GLenum dest_factor_alpha)
{
    CheckAndStartNewBatch();
    glBlendFuncSeparate(source_factor_rgb, dest_factor_rgb, source_factor_alpha, dest_factor_alpha);
}