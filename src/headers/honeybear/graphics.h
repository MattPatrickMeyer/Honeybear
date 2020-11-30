#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "sprite.h"

namespace Honeybear::Graphics
{
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 tex_coords;
    };

    struct QuadBatch
    {
        GLuint VAO;
        GLuint VBO;
        GLuint IB;

        uint32_t index_count;

        Vertex* buffer = nullptr;
        Vertex* buffer_ptr = nullptr;
    };

    struct FrameBuffer
    {
        GLuint FBO;
        GLuint tex_colour_buffer;
        float width;
        float height;
    };

    struct ScreenRenderData
    {
        GLuint quad_VAO;
        GLuint quad_VBO;
        GLuint quad_IB;
    };

    extern std::unordered_map<std::string, uint32_t> shaders;
    extern std::unordered_map<std::string, Texture> textures;
    extern std::map<uint8_t, uint32_t> bound_textures;
    extern std::unordered_map<uint32_t, Sprite> sprites;
    extern std::vector<FrameBuffer> frame_buffers;
    extern uint32_t current_frame_buffer_index;

    extern std::string activated_shader_id;

    extern GLFWwindow* window;

    extern QuadBatch quad_batch;
    extern ScreenRenderData screen_render_data;

    void Init(uint32_t window_width, uint32_t window_height, const std::string& window_title);
    void InitScreenRenderData();
    void Clear();
    void SwapBuffers();

    void LoadShader(const std::string& shader_id, const std::string& vertex_file_name, const std::string& fragment_file_name);
    void ActivateShader(const std::string& shader_id);
    void ActivateShader(const std::string& shader_id, const glm::mat4& projection);
    void DeactivateShader();
    void SetShaderProjection(const std::string& shader_id, const glm::mat4& projection);
    void SetMatrix4(const std::string& shader_id, const std::string& uniform_name, const glm::mat4& matrix);

    void LoadTexture(const std::string& texture_id, const std::string& texture_file_name, const FilterType filter_type);
    void BindTexture(const std::string& texture_id, const uint8_t texture_unit);

    void CreateSprite(const uint32_t sprite_id, const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h);
    void DrawSprite(const Sprite& sprite, glm::vec2 position, const uint32_t frame_buffer_index);

    //uint32_t AddFrameBuffer();
    uint32_t AddFrameBuffer(uint32_t width, uint32_t height);
    void RenderFrameBuffer(const uint32_t frame_buffer_index);
    void BindFrameBuffer(const uint32_t frame_buffer_index);

    void InitQuadBatch();
    void BeginBatch();
    void EndBatch();
    void FlushBatch();
};

#endif