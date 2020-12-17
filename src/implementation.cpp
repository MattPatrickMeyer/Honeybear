#include <Windows.h>
#include <iostream>
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
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    // int window_width = 2560;
    // int window_height = 1440;
    int window_width = 1920;
    int window_height = 1080;
    // int window_width = 1280;
    // int window_height = 720;

    Engine::Init(window_width, window_height, "Honeybear!");
    Engine::SetGameSize(640, 360);

    Graphics::LoadShader("test",            nullptr, "res/shaders/test.frag");
    Graphics::LoadShader("second_tex_test", nullptr, "res/shaders/second_tex_test.frag");

    SpriteSheet* sprites = Graphics::LoadSpriteSheet("sprites", "res/images/sprites.png", nullptr, nullptr, Graphics::NEAREST);
    SpriteSheet* ui =      Graphics::LoadSpriteSheet("ui",      "res/images/ui.png",      nullptr, nullptr, Graphics::LINEAR);

    Graphics::CreateSprite(1, sprites, 0,   96,  32, 32);
    Graphics::CreateSprite(2, sprites, 0,   128, 32, 32);
    Graphics::CreateSprite(3, sprites, 448, 128, 32, 32);
    Graphics::CreateSprite(4, ui,      0,   160, 16, 16);

    test_frame_buffer =         Graphics::AddFrameBuffer();
    another_test_frame_buffer = Graphics::AddFrameBuffer();
    ui_frame_buffer =           Graphics::AddFrameBuffer();

    std::cout << "test" << std::endl;
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
            Graphics::DrawSprite(Graphics::sprites[1], Vec2(x * 32.0f, y * 32.0f), test_frame_buffer);
        }
    }

    double x_pos, y_pos;
    Input::CursorGamePosition(Graphics::window, &x_pos, &y_pos);

    Graphics::DrawSprite(Graphics::sprites[3], Vec2(0 * 32.0f, 0 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], Vec2(6 * 32.0f, 5 * 32.0f), another_test_frame_buffer);
    Graphics::DrawSprite(Graphics::sprites[3], Vec2(7 * 32.0f + test, 5 * 32.0f), another_test_frame_buffer, Vec4(1.0f, 1.0f, 0.0f, 1.0f));

    Graphics::DrawSprite(Graphics::sprites[4], Vec2(100 + test, 100), Vec2(5.0f, 5.0f), ui_frame_buffer);

    Graphics::FillTriangle(Vec2(100.0f, 100.0f), Vec2(150.0f, 150.0f), Vec2(50.0f, 150.0f), ui_frame_buffer, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    Graphics::FillTriangle(Vec2(200.0f, 200.0f), Vec2(250.0f, 250.0f), Vec2(150.0f, 250.0f), ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Graphics::FillCircle(Vec2(300.0f + test, 300.0f), 50.0f, another_test_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    //Graphics::FillCircle(Vec2(x_pos, y_pos), 20.0f, another_test_frame_buffer, Vec4(0.0f, 0.0f, 1.0f, 1.0f));

    Graphics::FillRectangle(50.0f + test, 300.0f, 100.0f, 50.0f, another_test_frame_buffer, Vec4(1.0f, 0.5f, 0.0f, 1.0f));

    std::vector<Vec2> points;
    points.emplace_back(0.0f, 0.0f);
    points.emplace_back(50.0f, 50.0f);
    points.emplace_back(40.0f, 75.0f);
    points.emplace_back(0.0f, 100.0f);
    points.emplace_back(-50.0f, 50.0f);
    for(size_t i = 0; i < points.size(); ++i)
    {
        points[i].x += x_pos;
        points[i].y += y_pos;
    }

    Graphics::FillConvexPoly(points, another_test_frame_buffer, Vec4(1.0f, 0.0f, 1.0f, 1.0f));

    Graphics::DeactivateShader();

    Graphics::ActivateShader("second_tex_test");
    Graphics::SetShaderFramebufferTexture("second_tex_test", "second_image", another_test_frame_buffer, 1);
    Graphics::FillRectangle(200.0f, 0.0f, 192.0f, 108.0f, ui_frame_buffer, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Graphics::DeactivateShader();

    Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);
    Graphics::RenderFrameBuffer(ui_frame_buffer);
}

bool full_screen = false;
bool v_sync = false;

void Implementation::HandleInput()
{
    if(Input::IsKeyHeld(Input::KEY_D))
    {
        key_down = true;
    }
    else
    {
        key_down = false;
    }
    if(Input::WasKeyPressed(Input::KEY_F1))
    {
        Graphics::ChangeResolution(1280, 720);
    }
    if(Input::WasKeyPressed(Input::KEY_F2))
    {
        Graphics::ChangeResolution(1920, 1080);
    }
    if(Input::WasKeyPressed(Input::KEY_F3))
    {
        Graphics::ChangeResolution(2560, 1440);
    }
    if(Input::WasKeyPressed(Input::KEY_F12))
    {
        full_screen = !full_screen;
        Graphics::ToggleFullscreen(full_screen);
    }
    if(Input::WasKeyPressed(Input::KEY_F10))
    {
        v_sync = !v_sync;
        Graphics::ToggleVSync(v_sync);
    }
}