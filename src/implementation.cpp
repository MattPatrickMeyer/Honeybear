//#include <Windows.h>
#include <iostream>
#include <algorithm>
#include "implementation.h"
#include "honeybear/graphics.h"
#include "honeybear/input.h"
#include "honeybear/geometry.h"
#include "honeybear/stb_image.h"

using namespace Honeybear;

uint32_t test_frame_buffer;
uint32_t another_test_frame_buffer;
uint32_t ui_frame_buffer;

Texture* palette;

Implementation::Implementation()
{
    // AllocConsole();
    // freopen("CONOUT$", "w", stdout);

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
    Graphics::LoadShader("msdf_font",       nullptr, "res/shaders/msdf_font.frag");

    // SpriteSheet* sprites = Graphics::LoadSpriteSheet("sprites", "res/images/sprites.png", nullptr, nullptr, Graphics::NEAREST);
    // SpriteSheet* ui =      Graphics::LoadSpriteSheet("ui",      "res/images/ui.png",      nullptr, nullptr, Graphics::LINEAR);

    // Graphics::CreateSprite(1, sprites, 0,   96,  32, 32);
    // Graphics::CreateSprite(2, sprites, 0,   128, 32, 32);
    // Graphics::CreateSprite(3, sprites, 448, 128, 32, 32);
    // Graphics::CreateSprite(4, ui,      0,   160, 16, 16);

    Graphics::LoadSpritesFile("res/images/sprites.txt", Graphics::NEAREST);
    Graphics::LoadSpritesFile("res/images/ui.txt", Graphics::LINEAR);

    test_frame_buffer =         Graphics::AddFrameBuffer();
    another_test_frame_buffer = Graphics::AddFrameBuffer();
    ui_frame_buffer =           Graphics::AddFrameBuffer();

    //Graphics::SetClearColour(Vec4(1.0f, 0.5f, 0.0f, 1.0f));
    //Texture* palette = Graphics::LoadTexture("res/images/sprites.png", Graphics::NEAREST);
    palette = Graphics::LoadTexture("res/images/palette.png", Graphics::NEAREST);

    Graphics::LoadMSDFFont("inconsolata", "res/fonts/inconsolata/inconsolata_msdf.png", "res/fonts/inconsolata/inconsolata_data.csv");
    Graphics::LoadMSDFFont("davida",      "res/fonts/davida/davida_msdf.png",           "res/fonts/davida/davida_data.csv");

    std::cout << stbi_failure_reason() << std::endl;
}

bool key_down = false;
float test = 0.0f;

void Implementation::Update(const float dt)
{
    if(key_down)
    {
        test += 10.0f * dt;
    }
    test += dt;
}

void Implementation::Draw()
{
    //Graphics::ActivateShader("default");

    // for(int x = 0; x < 60; ++x)
    // {
    //     for(int y = 0; y < 34; ++y)
    //     {
    //         Graphics::DrawSprite(Graphics::sprites[105], Vec2(x * 32.0f, y * 32.0f), test_frame_buffer);
    //     }
    // }

    Vec2 mouse_pos;
    Input::CursorGamePosition(Graphics::window, &mouse_pos);

    // Graphics::DrawSprite(*Graphics::GetSprite(998), Vec2(0 * 32.0f, 0 * 32.0f), another_test_frame_buffer);
    // Graphics::DrawSprite(*Graphics::GetSprite(998), Vec2(6 * 32.0f, 5 * 32.0f), another_test_frame_buffer);
    // Graphics::DrawSprite(*Graphics::GetSprite(998), Vec2(7 * 32.0f + test, 5 * 32.0f), another_test_frame_buffer, Vec4(1.0f, 1.0f, 0.0f, 1.0f));

    // Graphics::DrawSprite(*Graphics::GetSprite(8000), Vec2(100 + test, 100), Vec2(5.0f, 5.0f), ui_frame_buffer);

    // Graphics::FillTriangle(Vec2(100.0f, 100.0f), Vec2(150.0f, 150.0f), Vec2(50.0f, 150.0f), ui_frame_buffer, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // Graphics::FillTriangle(Vec2(200.0f, 200.0f), Vec2(250.0f, 250.0f), Vec2(150.0f, 250.0f), ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Graphics::DrawCircle(Vec2(300.0f + test, 300.0f), 50.0f, another_test_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    // Graphics::DrawLine(Vec2(0.0f, 0.0f), Vec2(100.0f, 100.0f), another_test_frame_buffer, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // Graphics::DrawLine(Vec2(100.0f, 100.0f), Vec2(150.0f, 100.0f), another_test_frame_buffer, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // Graphics::DrawRectangle(200.0f, 200.0f, 100.0f, 50.0f, another_test_frame_buffer, Vec4(1.0f));
    //Graphics::FillCircle(Vec2(x_pos, y_pos), 20.0f, another_test_frame_buffer, Vec4(0.0f, 0.0f, 1.0f, 1.0f));

    Graphics::FillRectangle(50.0f, 300.0f, 100.0f, 50.0f, another_test_frame_buffer, Vec4(1.0f, 0.5f, 0.0f, 1.0f));

    // std::vector<Vec2> points;
    // points.emplace_back(0.0f, 0.0f);
    // points.emplace_back(50.0f, 50.0f);
    // points.emplace_back(40.0f, 75.0f);
    // points.emplace_back(0.0f, 100.0f);
    // points.emplace_back(-50.0f, 50.0f);
    // for(size_t i = 0; i < points.size(); ++i)
    // {
    //     points[i].x += mouse_pos.x;
    //     points[i].y += mouse_pos.y;
    // }

    // Graphics::DrawPoly(points, another_test_frame_buffer, Vec4(1.0f, 0.0f, 1.0f, 1.0f));

    // Graphics::DeactivateShader();

    // Graphics::ActivateShader("second_tex_test");
    // Graphics::SetShaderTexture("second_tex_test", "second_image", palette->ID, 1);
    // Graphics::FillRectangle(200.0f, 0.0f, 192.0f, 108.0f, another_test_frame_buffer, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    // Graphics::DeactivateShader();

    // Graphics::RenderFrameBufferToFrameBuffer(another_test_frame_buffer, test_frame_buffer);

    Graphics::ActivateShader("msdf_font");
    Graphics::RenderText(std::to_string(test), Vec2(50.0f, 50.0f), "inconsolata", 20.0f, ui_frame_buffer);
    Graphics::RenderText("Honeybear!", mouse_pos, "davida", 20.0f, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    Graphics::RenderText("This is a test :)", Vec2(20.0f, 200.0f), "inconsolata", 20.0f, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    // Graphics::RenderText("Honeybear!", mouse_pos, "inconsolata", ui_frame_buffer);
    Graphics::DeactivateShader();

    // Graphics::RenderFrameBuffer(test_frame_buffer);
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