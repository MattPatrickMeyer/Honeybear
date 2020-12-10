#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Honeybear
{
    struct Sprite
    {
        std::string texture_id;
        float width;
        float height;
        float texture_x;
        float texture_y;
        float texture_w;
        float texture_h;
    };

    namespace Graphics
    {
        enum FilterType
        {
            NEAREST,
            LINEAR
        };

        struct Texture
        {
            GLuint ID;

            uint32_t width;
            uint32_t height;

            GLuint internal_format;
            GLuint image_format;
            GLuint wrap_s;
            GLuint wrap_t;
        };

        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 tex_coords;
            glm::vec4 colour;
        };

        struct Batch
        {
            GLuint VAO;
            GLuint VBO;
            GLuint IB;

            GLuint shape_texture;

            Vertex* buffer = nullptr;
            Vertex* buffer_ptr = nullptr;

            uint32_t* index_buffer = nullptr;
            uint32_t current_index_offset;
            uint32_t index_count;
        };

        struct FrameBuffer
        {
            GLuint FBO;
            GLuint tex_colour_buffer;
            float width;
            float height;
            float game_pixel_size;
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

        extern Batch batch;
        extern ScreenRenderData screen_render_data;

        void Init(uint32_t window_width, uint32_t window_height, const std::string& window_title);
        void InitScreenRenderData();
        void Clear();
        void ClearFrameBuffers();
        void SwapBuffers();

        void LoadShader(const std::string& shader_id, const std::string& vertex_file_name, const std::string& fragment_file_name);
        void ActivateShader(const std::string& shader_id);
        void ActivateShader(const std::string& shader_id, const glm::mat4& projection);
        void DeactivateShader();
        void SetShaderProjection(const std::string& shader_id, const glm::mat4& projection);
        void SetMatrix4(const std::string& shader_id, const std::string& uniform_name, const glm::mat4& matrix);

        void LoadTexture(const std::string& texture_id, const std::string& texture_file_name, const FilterType filter_type);
        void BindTexture(const std::string& texture_id, const uint8_t texture_unit);
        void BindTexture(const GLuint texture_id, const uint8_t texture_unit);

        void CreateSprite(const uint32_t sprite_id, const std::string& texture_id, float tex_x, float tex_y, float tex_w, float tex_h);
        void DrawSprite(const Sprite& sprite, glm::vec2 position, const uint32_t frame_buffer_index);
        void DrawSprite(const Sprite& sprite, glm::vec2 position, const uint32_t frame_buffer_index, const glm::vec4& colour);

        void FillTriangle(const glm::vec2& pos_a, const glm::vec2& pos_b, const glm::vec2& pos_c, const uint32_t frame_buffer_index, const glm::vec4& colour);
        void FillRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const glm::vec4& colour);
        void FillCircle(const glm::vec2& pos, const float radius, const uint32_t frame_buffer_index, const glm::vec4& colour);

        uint32_t AddFrameBuffer(uint32_t width, uint32_t height);
        void RenderFrameBuffer(const uint32_t frame_buffer_index);
        void BindFrameBuffer(const uint32_t frame_buffer_index);

        void InitBatchRenderer();
        void BeginBatch();
        void EndBatch();
        void FlushBatch();
        void DoBatchRenderSetUp(const uint32_t frame_buffer_index, const GLuint tex_id, const uint32_t num_indices);
    }
};

#endif