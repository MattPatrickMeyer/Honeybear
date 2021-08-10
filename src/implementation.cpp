#include <Windows.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include "implementation.h"
#include "honeybear/graphics.h"
#include "honeybear/input.h"
#include "honeybear/geometry.h"
#include "honeybear/stb_image.h"
#include "honeybear/engine.h"

using namespace Honeybear;

uint32_t test_frame_buffer;
uint32_t another_test_frame_buffer;
uint32_t ui_frame_buffer;
uint32_t little_frame_buffer;
uint32_t multi_sample_frame_buffer;

Texture* palette;

const float GAME_HEIGHT = 360.0f;

void Implementation::Init()
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
    //Engine::SetGameSize(640, 360);
    Engine::SetGameScale(window_height / GAME_HEIGHT);
    Engine::SetFixedTimeStep(1.0f / 240.0f);

    Engine::SetBeginFrameCallback(BeginFrame);
    Engine::SetDrawCallback(Draw);
    Engine::SetUpdateFixedCallback(UpdateFixed);
    Engine::SetInterpolateStateCallback(InterpolateState);

    Graphics::LoadShader("sprite",          nullptr, "res/shaders/sprite.frag");
    Graphics::LoadShader("second_tex_test", nullptr, "res/shaders/second_tex_test.frag");
    //Graphics::LoadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");
    //Graphics::LoadShader("msdf_font", "res/shaders/msdf_font.vert", "res/shaders/msdf_font.frag");

    // SpriteSheet* sprites = Graphics::LoadSpriteSheet("sprites", "res/images/sprites.png", nullptr, nullptr, Graphics::NEAREST);
    // SpriteSheet* ui =      Graphics::LoadSpriteSheet("ui",      "res/images/ui.png",      nullptr, nullptr, Graphics::LINEAR);

    Graphics::LoadSpritesFile("res/images/sprites.txt", Graphics::NEAREST);
    Graphics::LoadSpritesFile("res/images/ui.txt", Graphics::LINEAR);

    test_frame_buffer =         Graphics::AddFrameBuffer();
    another_test_frame_buffer = Graphics::AddFrameBuffer();
    ui_frame_buffer =           Graphics::AddFrameBuffer();
    little_frame_buffer =       Graphics::AddFrameBuffer(100.0f, 100.0f);
    multi_sample_frame_buffer = Graphics::AddMultiSampledFrameBuffer(8);

    Graphics::EnableBufferAutoScaling(test_frame_buffer);
    Graphics::EnableBufferAutoScaling(another_test_frame_buffer);
    Graphics::EnableBufferAutoScaling(multi_sample_frame_buffer);

    UpdateBuffers(window_width, window_height);

    Graphics::SetClearColour(ui_frame_buffer, Vec4(0.0f));
    Graphics::SetClearColour(Vec4(0.47f, 0.54f, 0.67f, 1.0f));

    //Graphics::SetClearColour(Vec4(1.0f, 0.5f, 0.0f, 1.0f));
    //Texture* palette = Graphics::LoadTexture("res/images/sprites.png", Graphics::NEAREST);
    palette = Graphics::LoadTexture("res/images/palette.png", Graphics::NEAREST);

    Graphics::LoadMSDFFont("roboto_mono", "res/fonts/roboto_mono/atlas.png", "res/fonts/roboto_mono/data.csv");

    std::cout << stbi_failure_reason() << std::endl;

    Engine::Run();
}

bool key_down = false;
float x_test = 0.0f;
float test = 0.0f;
float prev_test = 0.0f;
float angle = 0.0f;
float another_test = 20.0f;

void Implementation::UpdateFixed(const double dt)
{
    prev_test = test;
    //test += dt;
    angle += dt;
}

float inter_test = 0.0f;

void Implementation::Draw()
{
    Vec2 mouse_pos;
    Input::CursorGamePosition(&mouse_pos);
    //Input::CursorWindowPosition(&mouse_pos);

    int window_width, window_height;
    Graphics::GetResolution(&window_width, &window_height);

    float stats_font_size = 20.0f;

    float fps_width, fps_height;
    Graphics::CalcTextDimensions(std::to_string(Engine::average_fps), "roboto_mono", stats_font_size, &fps_width, &fps_height);

    float frame_time_width, frame_time_height;
    Graphics::CalcTextDimensions(std::to_string(Engine::last_frame_time), "roboto_mono", stats_font_size, &frame_time_width, &frame_time_height);

    float game_speed_width, game_speed_height;
    Graphics::CalcTextDimensions(std::to_string(Honeybear::game_speed), "roboto_mono", stats_font_size, &game_speed_width, &game_speed_height);

    Graphics::RenderText(std::to_string(test), Vec2(50.0f, 100.0f), "roboto_mono", 20.0f, test_frame_buffer, Vec4(1.0f, 1.0f, 1.0f, 0.3f));
    Graphics::RenderText(std::to_string(inter_test), Vec2(50.0f, 50.0f), "roboto_mono", 20.0f, test_frame_buffer, Vec4(1.0f, 1.0f, 1.0f, 0.3f));

    Graphics::RenderText("(" + std::to_string(mouse_pos.x) + ", " + std::to_string(mouse_pos.y) + ")", mouse_pos, "roboto_mono", 4.0f, test_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    // Graphics::RenderText("This is a test :)", Vec2(20.0f, 200.0f), "roboto_mono", another_test, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    // Graphics::RenderText(std::to_string(fps_width), Vec2(20.0f, 120.0f), "roboto_mono", 20.0f, ui_frame_buffer, Vec4(1.0f, 0.6f, 0.6f, 1.0f));
    Graphics::RenderText(std::to_string(Engine::average_fps), Vec2(window_width - fps_width, 0.0f), "roboto_mono", stats_font_size, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Graphics::RenderText(std::to_string(Engine::last_frame_time), Vec2(window_width - frame_time_width, fps_height), "roboto_mono", stats_font_size, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Graphics::RenderText(std::to_string(Honeybear::game_speed), Vec2(window_width - game_speed_width, frame_time_height + fps_height), "roboto_mono", stats_font_size, ui_frame_buffer, Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Graphics::FillRectangle(0.0f, 0.0f, 200.0f, 200.0f, little_frame_buffer, Vec4(1.0f));
    // Graphics::RenderFrameBufferToQuad(little_frame_buffer, 100.0f, 100.0f, 100.0f, 100.0f, ui_frame_buffer);

    //Graphics::ActivateShader("default");
    Graphics::FillTriangle(Vec2(0.0f + x_test, 0.0f), Vec2(150.0f + x_test, 200.0f), Vec2(0.0f + x_test, 200.0f), multi_sample_frame_buffer, Vec4(1.0f));
    Graphics::FillCircle(mouse_pos, 50.0f, multi_sample_frame_buffer, Vec4(1.0f));

    if(Input::IsMouseButtonHeld(Input::MOUSE_BUTTON_LEFT))
    {
        Graphics::ActivateShader("sprite");
        Graphics::RenderSprite(*Graphics::GetSprite(799), Vec2(100.0f), angle, Vec2(16.0f), another_test_frame_buffer, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        Graphics::DeactivateShader();
    }

    // Graphics::ActivateShader("second_tex_test");
    // Graphics::SetShaderFramebufferTexture("second_tex_test", "second_image", ui_frame_buffer, 1);
    // Graphics::FillRectangle(0.0f, 0.0f, 100.0f, 100.0f, another_test_frame_buffer, Vec4(1.0f));
    // Graphics::DeactivateShader();

    // Graphics::ActivateShader("test");
    // Graphics::FillTriangle(Vec2(100.0f / 3 + x_test, 0.0f), Vec2(250.0f / 3 + x_test, 200.0f / 3), Vec2(100.0f / 3 + x_test, 200.0f / 3), ui_frame_buffer, Vec4(1.0f));
    // Graphics::DeactivateShader();

    Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);
    Graphics::RenderFrameBuffer(ui_frame_buffer);
    //Graphics::RenderFrameBuffer(multi_sample_frame_buffer);
    //Graphics::RenderFrameBuffer(little_frame_buffer);
}

bool full_screen = false;
bool v_sync = false;

void Implementation::InterpolateState(const double t)
{
    Interp(inter_test, prev_test, test, t);
}

void Implementation::UpdateBuffers(const float window_width, const float window_height)
{
    Engine::SetGameScale(window_height / GAME_HEIGHT);

    Graphics::UpdateFrameBufferSize(test_frame_buffer, window_width, window_height);
    Graphics::UpdateFrameBufferSize(another_test_frame_buffer, window_width, window_height);
    Graphics::UpdateFrameBufferSize(ui_frame_buffer, window_width, window_height);
    Graphics::UpdateFrameBufferSize(multi_sample_frame_buffer, window_width, window_height);
}

void Implementation::BeginFrame()
{
    if(Input::WasKeyPressed(Input::KEY_ESC))
    {
        Engine::Quit();
    }
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
        UpdateBuffers(1280, 720);
    }
    if(Input::WasKeyPressed(Input::KEY_F2))
    {
        Graphics::ChangeResolution(1920, 1080);
        UpdateBuffers(1920, 1080);
    }
    if(Input::WasKeyPressed(Input::KEY_F3))
    {
        Graphics::ChangeResolution(2560, 1440);
        UpdateBuffers(2560, 1440);
    }
    if(Input::WasKeyPressed(Input::KEY_F5))
    {
        Graphics::ChangeResolution(2560, 1080);
        UpdateBuffers(2560, 1080);
    }
    if(Input::WasKeyPressed(Input::KEY_F6))
    {
        Graphics::ChangeResolution(3440, 1440);
        UpdateBuffers(3440, 1440);
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
    if(Input::WasKeyPressed(Input::KEY_F8))
    {
        Graphics::LoadSpritesFile("res/images/sprites.txt", Graphics::NEAREST);
    }
    if(Input::MouseScrolledUp())
    {
        test += 1.0f;
    }
    if(Input::MouseScrolledDown())
    {
        test -= 1.0f;
    }
}