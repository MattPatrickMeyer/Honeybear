#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "engine.h"
#include "graphics.h"
#include "input.h"

using namespace Honeybear;

float Honeybear::game_width;
float Honeybear::game_height;
float Honeybear::game_speed = 1.0f;
double Honeybear::total_elapsed_time = 0.0f;
int Honeybear::Engine::average_fps;
float Honeybear::Engine::last_frame_time;

namespace
{
    const float MAX_TIME_STEP = 1.0f / 240.0f;

    float fps_records[FPS_RECORD_COUNT];
    size_t fps_record_index = 0;
}

void Engine::Init(int window_width, int window_height, const std::string& window_title)
{
    game_width = window_width;
    game_height = window_height;
    Graphics::Init(window_width, window_height, window_title);
    Input::Init();
}

void Engine::Run()
{
    float last_update_time = Ticks();

    while(!glfwWindowShouldClose(Graphics::window))
    {
        ProcessInput();

        const float current_time = Ticks();
        const float frame_time = current_time - last_update_time;
        total_elapsed_time += frame_time * game_speed;
        last_frame_time = frame_time;

        last_update_time = current_time;

        if(frame_time > MAX_TIME_STEP)
        {
            float total_frame_time = frame_time;
            while(total_frame_time > 0.0f)
            {
                float dt = std::min(total_frame_time, MAX_TIME_STEP) * game_speed;
                total_frame_time -= MAX_TIME_STEP;
                Update(dt);
            }
        }
        else
        {
            float dt = frame_time * game_speed;
            Update(dt);
        }

        Render();

        // -- calc average frame rate --
        fps_records[fps_record_index] = 1.0f / frame_time;
        fps_record_index = (fps_record_index + 1) % 100;

        float total = 0.0f;
        for(size_t i = 0; i < FPS_RECORD_COUNT; ++i)
        {
            total += fps_records[i];
        }
        average_fps = int(total / FPS_RECORD_COUNT);
        // -----------------------------

        Input::BeginNewFrame();
        glfwPollEvents();
    }

    glfwTerminate();
}

double Engine::Ticks()
{
    // returns the time in seconds (since glfw was initialized)
    return glfwGetTime();
}

void Engine::Render()
{
    Graphics::Clear();
    Graphics::ClearFrameBuffers();

    Draw();

    Graphics::SwapBuffers();
}

void Engine::ProcessInput()
{
    if(Input::WasKeyPressed(Input::KEY_ESC))
    {
        glfwSetWindowShouldClose(Graphics::window, true);
    }

    HandleInput();
}

void Engine::SetGameSize(const float w, const float h)
{
    game_width = w;
    game_height = h;
}