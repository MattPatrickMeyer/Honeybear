//#include <Windows.h>
#include <math.h>
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
uint32_t little_frame_buffer;
uint32_t multi_sample_frame_buffer;

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

    // SpriteSheet* sprites = Graphics::LoadSpriteSheet("sprites", "res/images/sprites.png", nullptr, nullptr, Graphics::NEAREST);
    // SpriteSheet* ui =      Graphics::LoadSpriteSheet("ui",      "res/images/ui.png",      nullptr, nullptr, Graphics::LINEAR);

    Graphics::LoadSpritesFile("res/images/sprites.txt", Graphics::NEAREST);
    Graphics::LoadSpritesFile("res/images/ui.txt", Graphics::LINEAR);

    test_frame_buffer =         Graphics::AddFrameBuffer();
    another_test_frame_buffer = Graphics::AddFrameBuffer();
    ui_frame_buffer =           Graphics::AddFrameBuffer();
    little_frame_buffer =       Graphics::AddFrameBuffer(100.0f, 100.0f, false);
    multi_sample_frame_buffer = Graphics::AddMultiSampledFrameBuffer(8);

    //Graphics::SetClearColour(Vec4(1.0f, 0.5f, 0.0f, 1.0f));
    //Texture* palette = Graphics::LoadTexture("res/images/sprites.png", Graphics::NEAREST);
    palette = Graphics::LoadTexture("res/images/palette.png", Graphics::NEAREST);

    Graphics::LoadMSDFFont("roboto_mono",   "res/fonts/roboto_mono/atlas.png", "res/fonts/roboto_mono/data.csv");

    std::cout << stbi_failure_reason() << std::endl;
}

bool key_down = false;
float x_test = 0.0f;
float test = 0.0f;
float angle = 0.0f;
float another_test = 20.0f;

void Implementation::Update(const float dt)
{
    if(key_down)
    {
        x_test += 10.0f * dt;
    }
    test += dt;
    angle += dt;
    another_test = (std::sin(angle) * 200) + 200;
}

void Implementation::Draw()
{
    Vec2 mouse_pos;
    Input::CursorGamePosition(Graphics::window, &mouse_pos);

    float fps_width, fps_height;
    Graphics::CalcTextDimensions(std::to_string(Engine::average_fps), "roboto_mono", 5.0f, &fps_width, &fps_height);

    float frame_time_width, frame_time_height;
    Graphics::CalcTextDimensions(std::to_string(Engine::last_frame_time), "roboto_mono", 5.0f, &frame_time_width, &frame_time_height);

    float game_speed_width, game_speed_height;
    Graphics::CalcTextDimensions(std::to_string(Honeybear::game_speed), "roboto_mono", 5.0f, &game_speed_width, &game_speed_height);

    //Graphics::ActivateShader("msdf_font");
    Graphics::RenderText(std::to_string(test), Vec2(50.0f, 50.0f), "roboto_mono", 20.0f, ui_frame_buffer);
    Graphics::RenderText("(" + std::to_string(mouse_pos.x) + ", " + std::to_string(mouse_pos.y) + ")", mouse_pos, "roboto_mono", 20.0f, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    Graphics::RenderText("This is a test :)", Vec2(20.0f, 200.0f), "roboto_mono", another_test, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    Graphics::RenderText(std::to_string(fps_width), Vec2(20.0f, 120.0f), "roboto_mono", 20.0f, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    Graphics::RenderText(std::to_string(Engine::average_fps), Vec2(Honeybear::game_width - fps_width, 0.0f), "roboto_mono", 5.0f, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Graphics::RenderText(std::to_string(Engine::last_frame_time), Vec2(Honeybear::game_width - frame_time_width, fps_height), "roboto_mono", 5.0f, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Graphics::RenderText(std::to_string(Honeybear::game_speed), Vec2(Honeybear::game_width - game_speed_width, frame_time_height + fps_height), "roboto_mono", 5.0f, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    //Graphics::DeactivateShader();

    // Graphics::FillRectangle(0.0f, 0.0f, 200.0f, 200.0f, little_frame_buffer, Vec4(1.0f));
    // Graphics::RenderFrameBufferToQuad(little_frame_buffer, 100.0f, 100.0f, 100.0f, 100.0f, ui_frame_buffer);

    //Graphics::ActivateShader("default");
    Graphics::FillTriangle(Vec2(0.0f + x_test, 0.0f), Vec2(150.0f + x_test, 200.0f), Vec2(0.0f + x_test, 200.0f), multi_sample_frame_buffer, Vec4(1.0f));
    Graphics::FillCircle(mouse_pos, 50.0f, multi_sample_frame_buffer, Vec4(1.0f));

    // Graphics::ActivateShader("test");
    // Graphics::FillTriangle(Vec2(100.0f / 3 + x_test, 0.0f), Vec2(250.0f / 3 + x_test, 200.0f / 3), Vec2(100.0f / 3 + x_test, 200.0f / 3), ui_frame_buffer, Vec4(1.0f));
    // Graphics::DeactivateShader();

    // Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);
    Graphics::RenderFrameBuffer(ui_frame_buffer);
    Graphics::RenderFrameBuffer(multi_sample_frame_buffer);
    //Graphics::RenderFrameBuffer(little_frame_buffer);
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
    if(Input::WasKeyPressed(Input::KEY_MINUS))
    {
        Honeybear::game_speed = std::max(0.0f, game_speed - 0.1f);
    }
    if(Input::WasKeyPressed(Input::KEY_EQUAL))
    {
        Honeybear::game_speed = std::min(2.0f, game_speed + 0.1f);
    }
}