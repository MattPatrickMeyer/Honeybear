#include "implementation.h"
#include "honeybear/graphics.h"
#include "honeybear/input.h"

using namespace Honeybear;

uint32_t test_frame_buffer;
uint32_t another_test_frame_buffer;
uint32_t ui_frame_buffer;

float test = 0.0f;

Implementation::Implementation()
{
    // int window_width = 2560;
    // int window_height = 1440;
    int window_width = 1920;
    int window_height = 1080;
    // int window_width = 1280;
    // int window_height = 720;

    Engine::Init(window_width, window_height, "Honeybear!");
    Engine::SetGameSize(640, 360);

    Graphics::LoadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
    Graphics::LoadShader("test",    "res/shaders/default.vert", "res/shaders/test.frag");

    Graphics::LoadTexture("sprites", "res/images/sprites.png", Graphics::NEAREST);
    Graphics::LoadTexture("ui",      "res/images/ui.png",      Graphics::LINEAR);

    Graphics::CreateSprite(1, "sprites", 0,   96,  32, 32);
    Graphics::CreateSprite(2, "sprites", 0,   128, 32, 32);
    Graphics::CreateSprite(3, "sprites", 448, 128, 32, 32);
    Graphics::CreateSprite(4, "ui",      0,   160, 16, 16);

    test_frame_buffer =         Graphics::AddFrameBuffer(window_width, window_height);
    another_test_frame_buffer = Graphics::AddFrameBuffer(window_width, window_height);
    ui_frame_buffer =           Graphics::AddFrameBuffer(window_width, window_height);
}

bool key_down = false;

void Implementation::Update(const float dt)
{
    if(key_down)
    {
        test += 10.0f * dt;
    }
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

    double x_pos, y_pos;
    Input::CursorGamePosition(Graphics::window, &x_pos, &y_pos);

    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(0 * 32.0f, 0 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(6 * 32.0f, 5 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(7 * 32.0f + test, 5 * 32.0f), another_test_frame_buffer, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    Graphics::DrawSprite(Graphics::sprites[4], glm::vec2(100 + test, 100), ui_frame_buffer);

    Graphics::FillTriangle(glm::vec2(100.0f, 100.0f), glm::vec2(150.0f, 150.0f), glm::vec2(50.0f, 150.0f), ui_frame_buffer, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    Graphics::FillTriangle(glm::vec2(200.0f, 200.0f), glm::vec2(250.0f, 250.0f), glm::vec2(150.0f, 250.0f), ui_frame_buffer, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Graphics::FillCircle(glm::vec2(300.0f + test, 300.0f), 50.0f, another_test_frame_buffer, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Graphics::FillCircle(glm::vec2(x_pos, y_pos), 20.0f, another_test_frame_buffer, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    Graphics::FillRectangle(50.0f + test, 300.0f, 100.0f, 50.0f, another_test_frame_buffer, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

    Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);
    Graphics::RenderFrameBuffer(ui_frame_buffer);
}

void Implementation::HandleInput()
{
    if(Input::IsKeyHeld(Input::D_KEY))
    {
        key_down = true;
    }
    else
    {
        key_down = false;
    }
}