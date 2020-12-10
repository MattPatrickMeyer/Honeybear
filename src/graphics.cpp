#include <math.h>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "graphics.h"
#include "engine.h"
#include "stb_image.h"

using namespace Honeybear;

std::unordered_map<std::string, uint32_t> Graphics::shaders;
std::unordered_map<std::string, Graphics::Texture> Graphics::textures;
std::map<uint8_t, uint32_t> Graphics::bound_textures;
std::unordered_map<uint32_t, Sprite> Graphics::sprites;
std::vector<Graphics::FrameBuffer> Graphics::frame_buffers;
uint32_t Graphics::current_frame_buffer_index;
std::string Graphics::activated_shader_id = "default";
GLFWwindow* Graphics::window;
Graphics::Batch Graphics::batch;
Graphics::ScreenRenderData Graphics::screen_render_data;

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
    //glfwWindowHint(GLFW_SAMPLES, 4);

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

    int w_width, w_height;
    glViewport(0, 0, window_width, window_height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_POLYGON_SMOOTH);

    // vsync
    //glfwSwapInterval(1);

    InitScreenRenderData();
    InitBatchRenderer();
}

void Graphics::InitScreenRenderData()
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    glGenVertexArrays(1, &screen_render_data.quad_VAO);
    glBindVertexArray(screen_render_data.quad_VAO);

    glGenBuffers(1, &screen_render_data.quad_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, screen_render_data.quad_VBO);

    glm::vec4 colour;
    colour.r = 1.0f;
    colour.g = 1.0f;
    colour.b = 1.0f;
    colour.a = 1.0f;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour);

    // unbind everything
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    BeginBatch();
}

void Graphics::Clear()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::ClearFrameBuffers()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    for(size_t i = 0; i < frame_buffers.size(); ++i)
    {
        FrameBuffer* frame_buffer = &frame_buffers[i];
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Graphics::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void Graphics::LoadShader(const std::string& shader_id, const std::string& vertex_file_name, const std::string& fragment_file_name)
{
    uint32_t program_id;
    uint32_t vertex_shader_id;
    uint32_t fragment_shader_id;

    std::string vertex_code_str;
    std::string fragment_code_str;
    const char* vertex_code;
    const char* fragment_code;

    std::ifstream vertex_shader_file(vertex_file_name);
    std::ifstream fragment_shader_file(fragment_file_name);

    vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if(vertex_shader_file.is_open() && fragment_shader_file.is_open())
    {
        std::stringstream vertex_shader_stream;
        std::stringstream fragment_shader_stream;

        vertex_shader_stream << vertex_shader_file.rdbuf();
        fragment_shader_stream << fragment_shader_file.rdbuf();

        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_code_str = vertex_shader_stream.str();
        fragment_code_str = fragment_shader_stream.str();
    }
    else
    {
        // you're fucked
        return;
    }

    vertex_code = vertex_code_str.c_str();
    fragment_code = fragment_code_str.c_str();

    int success;
    char infoLog[512];

    // vertex shader compile and check
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_code, NULL);
    glCompileShader(vertex_shader_id);

    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader compile and check
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_code, NULL);
    glCompileShader(fragment_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // shader program link and check
    program_id = glCreateProgram();
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
}

void Graphics::ActivateShader(const std::string& shader_id)
{
    FrameBuffer* frame_buffer = &frame_buffers[current_frame_buffer_index];
    glm::mat4 projection = glm::ortho(0.0f, frame_buffer->width, frame_buffer->height, 0.0f, -1.0f, 1.0f);
    ActivateShader(shader_id, projection);
}

void Graphics::ActivateShader(const std::string& shader_id, const glm::mat4& projection)
{
    glUseProgram(shaders[shader_id]);
    SetShaderProjection(shader_id, projection);
    activated_shader_id = shader_id;
}

void Graphics::DeactivateShader()
{
    if(batch.index_count > 0)
    {
        EndBatch();
        FlushBatch();
    }

    ActivateShader("default");
}

void Graphics::SetShaderProjection(const std::string& shader_id, const glm::mat4& projection)
{
    Graphics::SetMatrix4(shader_id, "projection", projection);
}

void Graphics::SetMatrix4(const std::string& shader_id, const std::string& uniform_name, const glm::mat4& matrix)
{
    uint32_t program_ID = shaders[shader_id];
    glUniformMatrix4fv(glGetUniformLocation(program_ID, uniform_name.c_str()), 1, false, glm::value_ptr(matrix));
}

void Graphics::LoadTexture(const std::string& texture_id, const std::string& texture_file_name, const FilterType filter_type)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texture_file_name.c_str(), &width, &height, &nrChannels, 0);

    GLuint filter = GL_LINEAR;
    if(filter_type == NEAREST) filter = GL_NEAREST;

    Texture* texture = &textures[texture_id];

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
}

void Graphics::BindTexture(const std::string& texture_id, const uint8_t texture_unit)
{
    glActiveTexture(texture_units[texture_unit]);
    uint32_t ID = textures[texture_id].ID;
    glBindTexture(GL_TEXTURE_2D, ID);

    bound_textures[texture_unit] = ID;
}

void Graphics::BindTexture(const GLuint texture_id, const uint8_t texture_unit)
{
    glActiveTexture(texture_units[texture_unit]);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    bound_textures[texture_unit] = texture_id;
}

void Graphics::DrawSprite(const Sprite& sprite, glm::vec2 position, const uint32_t frame_buffer_index)
{
    glm::vec4 colour;
    colour.r = 1.0f;
    colour.g = 1.0f;
    colour.b = 1.0f;
    colour.a = 1.0f;

    DrawSprite(sprite, position, frame_buffer_index, colour);
}

void Graphics::DrawSprite(const Sprite& sprite, glm::vec2 position, const uint32_t frame_buffer_index, const glm::vec4& colour)
{
    int indices_count = 6;
    uint32_t texture_id = textures[sprite.texture_id].ID;

    DoBatchRenderSetUp(frame_buffer_index, texture_id, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // bottom right
    batch.buffer_ptr->position.x = (position.x + sprite.width) * pixel_size;
    batch.buffer_ptr->position.y = (position.y + sprite.height) * pixel_size;
    batch.buffer_ptr->tex_coords.x = sprite.texture_x + sprite.texture_w;
    batch.buffer_ptr->tex_coords.y = sprite.texture_y + sprite.texture_h;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top right
    batch.buffer_ptr->position.x = (position.x + sprite.width) * pixel_size;
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
    batch.buffer_ptr->position.y = (position.y + sprite.height) * pixel_size;
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

void Graphics::FillTriangle(const glm::vec2& pos_a, const glm::vec2& pos_b, const glm::vec2& pos_c, const uint32_t frame_buffer_index, const glm::vec4& colour)
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

void Graphics::FillRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const glm::vec4& colour)
{
    int indices_count = 6;

    DoBatchRenderSetUp(frame_buffer_index, batch.shape_texture, indices_count);

    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // bottom right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = (y + h) * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
    batch.buffer_ptr->tex_coords.y = 0.0f;
    batch.buffer_ptr->colour = colour;
    batch.buffer_ptr++;

    // top right
    batch.buffer_ptr->position.x = (x + w) * pixel_size;
    batch.buffer_ptr->position.y = y * pixel_size;
    batch.buffer_ptr->tex_coords.x = 0.0f;
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
    batch.buffer_ptr->tex_coords.y = 0.0f;
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

void Graphics::FillCircle(const glm::vec2& pos, const float radius, const uint32_t frame_buffer_index, const glm::vec4& colour)
{
    float pixel_size = frame_buffers[frame_buffer_index].game_pixel_size;

    // -----------------------------
    // the below formula was taken from here: https://stackoverflow.com/questions/11774038/how-to-render-a-circle-with-as-few-vertices-as-possible
    float error = 0.25f;
    float th = std::acos(2 * ((1 - error / (radius * pixel_size)) * (1 - error / (radius * pixel_size))) - 1);
    int number_of_sides = std::ceil(2 * M_PI / th);
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

        float radians = degrees * (M_PI / 180.0f);

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

void Graphics::BeginBatch()
{
    batch.buffer_ptr = batch.buffer;
}

void Graphics::EndBatch()
{
    GLsizeiptr size = (uint8_t*)batch.buffer_ptr - (uint8_t*)batch.buffer;
    glBindBuffer(GL_ARRAY_BUFFER, batch.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch.buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.IB);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch.index_count * sizeof(uint32_t), batch.index_buffer);
}

void Graphics::FlushBatch()
{
    glBindVertexArray(batch.VAO);
    glDrawElements(GL_TRIANGLES, batch.index_count, GL_UNSIGNED_INT, nullptr);
    batch.index_count = 0;
    batch.current_index_offset = 0;
    glBindVertexArray(0);
}

void Graphics::DoBatchRenderSetUp(const uint32_t frame_buffer_index, const GLuint tex_id, const uint32_t num_indices)
{
    if(frame_buffer_index != current_frame_buffer_index)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    bool should_start_new_batch = false;
    bool should_bind_texture = false;

    //uint32_t texture_id = batch.shape_texture;
    if(bound_textures[0] != tex_id)
    {
        should_start_new_batch = batch.index_count > 0;
        should_bind_texture = true;
    }

    if(batch.index_count + num_indices > max_index_count)
    {
        should_start_new_batch = true;
    }

    if(should_start_new_batch)
    {
        EndBatch();
        FlushBatch();
        BeginBatch();
    }

    if(should_bind_texture)
    {
        BindTexture(tex_id, 0);
    }
}

void Graphics::CreateSprite(const uint32_t sprite_id, const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h)
{
    sprites[sprite_id].texture_id = texture_id;
    sprites[sprite_id].width = tex_w;
    sprites[sprite_id].height = tex_h;

    Texture* texture = &textures[texture_id];
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
    if(batch.index_count > 0)
    {
        EndBatch();
        FlushBatch();
    }

    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);
    glViewport(0, 0, frame_buffer->width, frame_buffer->height);
    current_frame_buffer_index = frame_buffer_index;

    // whenever we bind a new frame buffer, start a new batch
    BeginBatch();

    // make sure the shader projection matrix is set up
    glm::mat4 projection = glm::ortho(0.0f, frame_buffer->width, frame_buffer->height, 0.0f, -1.0f, 1.0f);
    SetShaderProjection(activated_shader_id, projection);
}

uint32_t Graphics::AddFrameBuffer(uint32_t width, uint32_t height)
{
    //FrameBuffer frame_buffer;
    frame_buffers.push_back(FrameBuffer());
    uint32_t frame_buffer_index = frame_buffers.size() - 1;
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    glGenFramebuffers(1, &frame_buffer->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);

    glGenTextures(1, &frame_buffer->tex_colour_buffer);
    glBindTexture(GL_TEXTURE_2D, frame_buffer->tex_colour_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->tex_colour_buffer, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frame_buffer->width = width;
    frame_buffer->height = height;
    frame_buffer->game_pixel_size = width / Honeybear::game_width;

    // if this is the first frame buffer, bind it?
    if(frame_buffer_index == 0)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    return frame_buffer_index;
}

void Graphics::RenderFrameBuffer(const uint32_t frame_buffer_index)
{
    FrameBuffer* frame_buffer = &frame_buffers[frame_buffer_index];

    // before we render the frame buffer to the screen, make sure all batched quads have been flushed to their buffer
    if(batch.index_count > 0)
    {
        EndBatch();
        FlushBatch();
    }

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    glm::mat4 projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -1.0f, 1.0f);
    SetShaderProjection(activated_shader_id, projection);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_buffer->tex_colour_buffer);
    glBindVertexArray(screen_render_data.quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}