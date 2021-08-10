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

        uint32_t id;
        std::string name;

        int width;
        int height;

        float texture_x;
        float texture_y;
        float texture_w;
        float texture_h;
    };

    enum SpriteSheetLayer
    {
        DIFFUSE,
        SPECULAR,
        NORMAL
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
            Vec3 position;
            Vec2 tex_coords;
            Vec4 colour;
        };

        enum BatchType
        {
            TEXTURE,
            LINES,
            FONT
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

            bool draw_lines = false;
            BatchType batch_type = TEXTURE;
        };

        struct FrameBuffer
        {
            // the main frame buffer and texture buffer
            GLuint FBO;
            GLuint tex_colour_buffer;

            // the intermediate frame buffer and texture buffer used for rendering multisampled buffer
            GLuint intermediate_FBO;
            GLuint intermediate_tex_colour_buffer;

            GLuint RBO;

            GLuint quad_VAO;
            GLuint quad_VBO;
            GLuint quad_IB;

            float width;
            float height;
            bool use_auto_scaling;
            //bool mapped_to_window_resolution; // todo: maybe use this in the future
            bool multisampled;
            bool resolved;
            bool depth_testing_enabled;
            uint32_t samples;
            Vec4 clear_colour;
        };

        struct UniformBlocks
        {
            GLuint matrices;
        };

        struct ScreenRenderData
        {
            GLuint quad_VAO;
            GLuint quad_VBO;
            GLuint quad_IB;

            int width;
            int height;
        };

        struct Bounds
        {
            float left;
            float right;
            float top;
            float bottom;
        };

        struct MSDF_CharData
        {
            int unicode;
            float advance;
            Bounds plane_bounds;
            Bounds atlas_bounds;
        };

        struct MSDF_FontData
        {
            int id;
            int index;
            int x;
            int y;
            int width;
            int height;
            int x_offset;
            int y_offset;
            int x_advance;
            int channel;
            std::string unihex;
        };

        struct MSDF_Font
        {
            Texture* texture;
            float tallest_char_height;
            std::map<int, MSDF_CharData> data;
            std::map<int, MSDF_FontData> font_data;
        };

        extern std::unordered_map<std::string, uint32_t> shaders;
        extern std::unordered_map<std::string, Texture> textures;
        extern std::map<uint8_t, uint32_t> bound_textures;
        extern std::unordered_map<std::string, SpriteSheet*> sprite_sheets;
        extern std::unordered_map<uint32_t, Sprite> sprites;
        extern std::unordered_map<std::string, MSDF_Font> msdf_fonts;
        extern std::vector<FrameBuffer> frame_buffers;
        extern uint32_t current_frame_buffer_index;
        extern UniformBlocks uniform_blocks;

        extern std::string activated_shader_id;

        extern GLFWwindow* window;

        extern Batch batch;
        extern ScreenRenderData screen_render_data;

        void Init(uint32_t window_width, uint32_t window_height, const std::string& window_title);
        void SetClearColour(const Vec4& colour);
        void SetClearColour(const uint32_t frame_buffer_index, const Vec4& colour);
        void Clear();
        void ClearFrameBuffers();
        void SwapBuffers();

        void InitScreenRenderData();
        void UpdateScreenRenderData();
        void UpdateScreenRenderData(const uint32_t frame_buffer_index, const float x, const float y, const float w, const float h);
        void InitUniformBlocks();

        void ChangeResolution(const uint32_t width, const uint32_t height);
        void GetResolution(int* width, int* height);
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
        void SetShaderFloatArray(const std::string& shader_id, const std::string& uniform_name, const float* values, const size_t n);
        void SetShaderIntArray(const std::string& shader_id, const std::string& uniform_name, const int* values, const size_t n);
        void SetShaderVec2Array(const std::string& shader_id, const std::string& uniform_name, const Vec2* values, const size_t n);
        void SetShaderVec3Array(const std::string& shader_id, const std::string& uniform_name, const Vec3* values, const size_t n);
        void SetShaderVec4Array(const std::string& shader_id, const std::string& uniform_name, const Vec4* values, const size_t n);
        void SetShaderTexture(const std::string& shader_id, const std::string& uniform_name, const GLuint texture_id, const uint8_t texture_unit);
        void SetShaderFramebufferTexture(const std::string& shader_id, const std::string& uniform_name, const uint32_t frame_buffer_index, const uint8_t texture_unit);

        Texture* LoadTexture(const std::string& texture_file_name, const FilterType filter_type);
        void BindTexture(const GLuint texture_id, const uint8_t texture_unit);

        void LoadSpritesFile(const std::string& file_name, const FilterType filter_type);
        SpriteSheet* LoadSpriteSheet(const std::string& sprite_sheet_name, const char* diffuse, const char* specular, const char* normal, const FilterType filter_type);
        void CreateSprite(const uint32_t sprite_id, SpriteSheet* sprite_sheet, int tex_x, int tex_y, int tex_w, int tex_h);
        Sprite* GetSprite(const uint32_t sprite_id);

        void RenderSprite(const Sprite& sprite, const Vec2& position, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec2& position, const Vec2& size, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec2& position, const float angle_degrees, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec2& position, const float angle_degrees, const Vec2& origin, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec2& position, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec2& position, const Vec2& size, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec3& position, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec3& position, const Vec2& size, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec3& position, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour = Vec4(1.0f));
        void RenderSprite(const Sprite& sprite, const Vec3& position, const Vec2& size, const float angle_degrees, const Vec2& origin, const uint32_t frame_buffer_index, const SpriteSheetLayer sprite_sheet_layer, const Vec4& colour = Vec4(1.0f));

        void DrawLine(const Vec2& start, const Vec2& end, const uint32_t frame_buffer_index, const Vec4& colour);
        void DrawRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour);
        void DrawCircle(const Vec2& pos, const float radius, const uint32_t frame_buffer_index, const Vec4& colour);
        void DrawPoly(const std::vector<Vec2>& points, const uint32_t frame_buffer_index, const Vec4& colour);

        void FillTriangle(const Vec2& pos_a, const Vec2& pos_b, const Vec2& pos_c, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillRectangle(const float x, const float y, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillRectangle(const float x, const float y, const float z, const float w, const float h, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillCircle(const Vec2& pos, const float radius, const uint32_t frame_buffer_index, const Vec4& colour);
        void FillConvexPoly(const std::vector<Vec2>& points, const uint32_t frame_buffer_index, const Vec4& colour);

        void DrawCustom(size_t num_verts, Vec3* positions, Vec2* tex_coords, Vec4* colours, size_t num_indices, int* indices, const uint32_t frame_buffer_index);
        void FillCustom(size_t num_verts, Vec3* positions, Vec2* tex_coords, Vec4* colours, size_t num_indices, int* indices, const uint32_t frame_buffer_index, const int texture_id = -1);

        uint32_t AddFrameBuffer();
        uint32_t AddFrameBuffer(const uint32_t width, const uint32_t height);
        uint32_t AddMultiSampledFrameBuffer(const uint32_t samples = 4);
        uint32_t AddMultiSampledFrameBuffer(const uint32_t width, const uint32_t height, const uint32_t samples = 4);
        uint32_t GetFrameBufferTextureID(const uint32_t frame_buffer_index);
        void EnableBufferAutoScaling(const uint32_t frame_buffer_index);
        void DisableBufferAutoScaling(const uint32_t frame_buffer_index);
        void AttachDepthBuffer(const uint32_t frame_buffer_index);
        void ResolveMultiSampledFrameBuffer(const uint32_t frame_buffer_index);
        void RenderFrameBuffer(const uint32_t frame_buffer_index);
        void RenderFrameBuffer(const uint32_t frame_buffer_index, const Vec2& offset);
        void RenderFrameBuffer(const uint32_t frame_buffer_index, const float src_x, const float src_y, const float src_w, const float src_h);
        void RenderFrameBufferToFrameBuffer(const uint32_t source_frame_buffer_index, const uint32_t dest_frame_buffer_index);
        void RenderFrameBufferToQuad(const uint32_t source_frame_buffer_index, const float x, const float y, const float w, const float h, const uint32_t dest_frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void BindFrameBuffer(const uint32_t frame_buffer_index);
        void UpdateFrameBufferSize(const uint32_t frame_buffer_index, const uint32_t width, const uint32_t height);

        void EnableBlending();
        void DisableBlending();
        void SetBlendFunction(GLenum source_factor, GLenum dest_factor);
        void SetBlendFunctionSeperate(GLenum source_factor_rgb, GLenum dest_factor_rgb, GLenum source_factor_alpha, GLenum dest_factor_alpha);

        void EnableDepthTesting();
        void DisableDepthTesting();

        void EnableScissorTesting();
        void DisableScissorTesting();
        void SetScissorRegion(const int x, const int y, const int width, const int height);
        void SetScissorRegion(const uint32_t frame_buffer_index, const int x, const int y, const int width, const int height);

        void InitBatchRenderer();
        void BeginBatch();
        void EndBatch();
        void FlushBatch();
        void CheckAndStartNewBatch();
        void DoBatchRenderSetUp(const uint32_t frame_buffer_index, const GLuint tex_id, const uint32_t num_indices, BatchType batch_type = TEXTURE);

        MSDF_Font* LoadMSDFFont(const std::string& font_id, const std::string& font_atlas_file_name, const std::string& font_data_file_name);
        void RenderText(const std::string& text, const Vec2& position, const std::string& font_id, const float size, const uint32_t frame_buffer_index, const Vec4& colour = Vec4(1.0f));
        void CalcTextDimensions(const std::string& text, const std::string& font_id, const float size, float* width, float* height);
    }
};

#endif
