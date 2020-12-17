#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "maths.h"

namespace Honeybear
{
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

    struct SpriteSheet
    {
        Texture* diffuse;
        Texture* specular;
        Texture* normal;
    };

    struct Sprite
    {
        SpriteSheet* sprite_sheet = nullptr;

        int width;
        int height;

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

        struct Vertex
        {
            Vec2 position;
            Vec2 tex_coords;
            Vec4 colour;
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
            bool mapped_to_window_resolution;
        };

        struct ScreenRenderData
        {
            GLuint quad_VAO;
            GLuint quad_VBO;
            GLuint quad_IB;

            int width;
            int height;
        };

        extern std::unordered_map<std::string, uint32_t> shaders;
        extern std::unordered_map<std::string, Texture> textures;
        extern std::map<uint8_t, uint32_t> bound_textures;
        extern std::unordered_map<std::string, SpriteSheet*> sprite_sheets;
        extern std::unordered_map<uint32_t, Sprite> sprites;
        extern std::vector<FrameBuffer> frame_buffers;
        extern uint32_t current_frame_buffer_index;

        extern std::string activated_shader_id;

        extern GLFWwindow* window;

        extern Batch batch;
        extern ScreenRenderData screen_render_data;

        void Init(uint32_t window_width, uint32_t window_height, const std::string& window_title);
        void Clear();
        void ClearFrameBuffers();
        void SwapBuffers();

        void InitScreenRenderData();
        void UpdateScreenRenderData();

        void ChangeResolution(const uint32_t width, const uint32_t height);
        void ToggleFullscreen(const bool enabled);
        void ToggleVSync(const bool enabled);

        void LoadShader(const std::string& shader_id, const char* vertex_file_name, const char* fragment_file_name);
        void CreateShaderProgram(const std::string& shader_id, const char* vertex_code, const char* fragment_code);
        void ActivateShader(const std::string& shader_id);
        void DeactivateShader();

        void SetShaderProjection(const std::string& shader_id, const float left, const float right, const float bottom, const float top, const float z_near, const float z_far);

        void SetShaderFloat(const std::string& shader_id, const std::string& uniform_name, const float value);
        void SetShaderInt(const std::string& shader_id, const std::string& uniform_name, const int value);
        void SetShaderVec2(const std::string& shader_id, const std::string& uniform_name, const Vec2& value);
        void SetShaderVec3(const std::string& shader_id, const std::string& uniform_name, const Vec3& value);
        void SetShaderVec4(const std::string& shader_id, const std::string& uniform_name, const Vec4& value);
        void SetShaderTexture(const std::string& shader_id, const std::string& uniform_name, const GLuint texture_id, const uint8_t texture_unit);
        void SetShaderFramebufferTexture(const std::string& shader_id, const std::string& uniform_name, const uint32_t frame_buffer_index, const uint8_t texture_unit);

        Texture* LoadTexture(const std::string& texture_file_name, const FilterType filter_type);
        void BindTexture(const GLuint texture_id, const uint8_t texture_unit);

        void LoadSpritesFile(const std::string& file_name, const FilterType filter_type);
        SpriteSheet* LoadSpriteSheet(const std::string& sprite_sheet_name, const char* diffuse, const char* specular, const char* normal, const FilterType filter_type);
        void CreateSprite(const uint32_t sprite_id, SpriteSheet* sprite_sheet, int tex_x, int tex_y, int tex_w, int tex_h);
        void DrawSprite(const Sprite& sprite, const Vec2 position, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void DrawSprite(const Sprite& sprite, const Vec2 position, const Vec2 size, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));

        void FillTriangle(const Vec2& pos_a, const Vec2& pos_b, const Vec2& pos_c, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillCircle(const Vec2& pos, const float radius, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillConvexPoly(const std::vector<Vec2>& points, const uint32_t frame_buffer_index, const Vec4& colour);

        uint32_t AddFrameBuffer();
        uint32_t AddFrameBuffer(const uint32_t width, const uint32_t height, const bool mapped_to_window_resolution = false);
        void RenderFrameBuffer(const uint32_t frame_buffer_index);
        void BindFrameBuffer(const uint32_t frame_buffer_index);
        void UpdateFrameBufferSize(const uint32_t frame_buffer_index, const uint32_t width, const uint32_t height);

        void InitBatchRenderer();
        void BeginBatch();
        void EndBatch();
        void FlushBatch();
        void DoBatchRenderSetUp(const uint32_t frame_buffer_index, const GLuint tex_id, const uint32_t num_indices);
    }
};

#endif