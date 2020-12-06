#include "implementation.h"
#include "honeybear/graphics.h"

using namespace Honeybear;

uint32_t test_frame_buffer;
uint32_t another_test_frame_buffer;
uint32_t ui_frame_buffer;

float test = 0.0f;

Implementation::Implementation()
{
    // int window_width = 2560;
    // int window_height = 1440;
    // int window_width = 1920;
    // int window_height = 1080;
    int window_width = 1280;
    int window_height = 720;
    Engine::Init(window_width, window_height, "Honeybear!");

    Graphics::LoadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
    Graphics::LoadShader("test",    "res/shaders/default.vert", "res/shaders/test.frag");

    Graphics::LoadTexture("sprites", "res/images/sprites.png", NEAREST);
    Graphics::LoadTexture("ui",      "res/images/ui.png",      LINEAR);

    Graphics::CreateSprite(1, "sprites", 0,   96,  32, 32);
    Graphics::CreateSprite(2, "sprites", 0,   128, 32, 32);
    Graphics::CreateSprite(3, "sprites", 448, 128, 32, 32);
    Graphics::CreateSprite(4, "ui",      0,   160, 16, 16);

    test_frame_buffer =         Graphics::AddFrameBuffer(window_width, window_height);
    another_test_frame_buffer = Graphics::AddFrameBuffer(window_width, window_height);
    ui_frame_buffer =           Graphics::AddFrameBuffer(window_width, window_height);
}

void Implementation::Update(const float dt)
{
    test += 50.0f * dt;
}

void Implementation::Draw()
{
    Graphics::ActivateShader("default");

    for(int x = 0; x < 60; ++x)
    {
        for(int y = 0; y < 34; ++y)
        {
            Graphics::DrawSprite(Graphics::sprites[1], glm::vec2(x * 32.0f, y * 32.0f), test_frame_buffer);
        }
    }

    glm::vec4 colour(1.0f, 0.0f, 0.0f, 1.0f);

    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(0 * 32.0f, 0 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(6 * 32.0f, 5 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(7 * 32.0f + test, 5 * 32.0f), another_test_frame_buffer, colour);

    Graphics::DrawSprite(Graphics::sprites[4], glm::vec2(100 + test, 100), ui_frame_buffer);

    Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);
    Graphics::RenderFrameBuffer(ui_frame_buffer);
}