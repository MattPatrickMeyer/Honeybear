#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "graphics.h"
#include "stb_image.h"

using namespace Honeybear;

std::unordered_map<std::string, uint32_t> Graphics::shaders;
std::unordered_map<std::string, Texture> Graphics::textures;
std::map<uint8_t, uint32_t> Graphics::bound_textures;
std::unordered_map<uint32_t, Sprite> Graphics::sprites;
std::vector<Graphics::FrameBuffer> Graphics::frame_buffers;
uint32_t Graphics::current_frame_buffer_index;
std::string Graphics::activated_shader_id = "default";
GLFWwindow* Graphics::window;
Graphics::QuadBatch Graphics::quad_batch;
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

const int max_quad_count = 10000;
const int max_vertex_count = max_quad_count * 4;
const int max_index_count = max_quad_count * 6;

void Graphics::Init(uint32_t window_width, uint32_t window_height, const std::string& window_title)
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitScreenRenderData();
    InitQuadBatch();
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

void Graphics::InitQuadBatch()
{
    // set up quad batch
    quad_batch.buffer = new Vertex[max_vertex_count];

    glGenVertexArrays(1, &quad_batch.VAO);
    glBindVertexArray(quad_batch.VAO);

    glGenBuffers(1, &quad_batch.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, quad_batch.VBO);
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

    // todo: this should just be set up once and reused
    uint32_t indices[max_index_count];
    uint32_t offset = 0;
    for(size_t i = 0; i < max_index_count; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 3 + offset;

        indices[i + 3] = 1 + offset;
        indices[i + 4] = 2 + offset;
        indices[i + 5] = 3 + offset;

        offset += 4;
    }

    glGenBuffers(1, &quad_batch.IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_batch.IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    if(quad_batch.index_count > 0)
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
    //stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(texture_file_name.c_str(), &width, &height, &nrChannels, 0);
    textures[texture_id].Generate(width, height, data, filter_type);
    stbi_image_free(data);
}

void Graphics::BindTexture(const std::string& texture_id, const uint8_t texture_unit)
{
    glActiveTexture(texture_units[texture_unit]);
    uint32_t ID = textures[texture_id].ID;
    glBindTexture(GL_TEXTURE_2D, ID);

    bound_textures[texture_unit] = ID;
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
    if(frame_buffer_index != current_frame_buffer_index)
    {
        BindFrameBuffer(frame_buffer_index);
    }

    bool start_new_batch = false;
    bool bind_texture = false;
    uint32_t texture_id = textures[sprite.texture_id].ID;
    if(bound_textures[0] != texture_id)
    {
        start_new_batch = quad_batch.index_count > 0;
        bind_texture = true;
    }

    if(quad_batch.index_count >= max_index_count)
    {
        start_new_batch = true;
    }

    if(start_new_batch)
    {
        EndBatch();
        FlushBatch();
        BeginBatch();
    }

    if(bind_texture)
    {
        BindTexture(sprite.texture_id, 0);
    }

    // bottom right
    quad_batch.buffer_ptr->position.x = position.x + sprite.width;
    quad_batch.buffer_ptr->position.y = position.y + sprite.height;
    quad_batch.buffer_ptr->tex_coords.x = sprite.texture_x + sprite.texture_w;
    quad_batch.buffer_ptr->tex_coords.y = sprite.texture_y + sprite.texture_h;
    quad_batch.buffer_ptr->colour = colour;
    quad_batch.buffer_ptr++;

    // top right
    quad_batch.buffer_ptr->position.x = position.x + sprite.width;
    quad_batch.buffer_ptr->position.y = position.y;
    quad_batch.buffer_ptr->tex_coords.x = sprite.texture_x + sprite.texture_w;
    quad_batch.buffer_ptr->tex_coords.y = sprite.texture_y;
    quad_batch.buffer_ptr->colour = colour;
    quad_batch.buffer_ptr++;

    // top left
    quad_batch.buffer_ptr->position.x = position.x;
    quad_batch.buffer_ptr->position.y = position.y;
    quad_batch.buffer_ptr->tex_coords.x = sprite.texture_x;
    quad_batch.buffer_ptr->tex_coords.y = sprite.texture_y;
    quad_batch.buffer_ptr->colour = colour;
    quad_batch.buffer_ptr++;

    // bottom left
    quad_batch.buffer_ptr->position.x = position.x;
    quad_batch.buffer_ptr->position.y = position.y + sprite.height;
    quad_batch.buffer_ptr->tex_coords.x = sprite.texture_x;
    quad_batch.buffer_ptr->tex_coords.y = sprite.texture_y + sprite.texture_h;
    quad_batch.buffer_ptr->colour = colour;
    quad_batch.buffer_ptr++;

    quad_batch.index_count += 6;
}

void Graphics::BeginBatch()
{
    quad_batch.buffer_ptr = quad_batch.buffer;
}

void Graphics::EndBatch()
{
    GLsizeiptr size = (uint8_t*)quad_batch.buffer_ptr - (uint8_t*)quad_batch.buffer;
    glBindBuffer(GL_ARRAY_BUFFER, quad_batch.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, quad_batch.buffer);
}

void Graphics::FlushBatch()
{
    glBindVertexArray(quad_batch.VAO);
    glDrawElements(GL_TRIANGLES, quad_batch.index_count, GL_UNSIGNED_INT, nullptr);
    quad_batch.index_count = 0;
    glBindVertexArray(0);
}

void Graphics::CreateSprite(const uint32_t sprite_id, const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h)
{
    sprites[sprite_id].Init(texture_id, tex_x, tex_y, tex_w, tex_h);
}

void Graphics::BindFrameBuffer(const uint32_t frame_buffer_index)
{
    // flush any batched quads to the previous frame buffer
    if(quad_batch.index_count > 0)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer->tex_colour_buffer, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    frame_buffer->width = width;
    frame_buffer->height = height;

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
    if(quad_batch.index_count > 0)
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