#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "engine.h"
#include "graphics.h"
#include "input.h"

using namespace Honeybear;

float Honeybear::game_width;
float Honeybear::game_height;

const float MAX_TIME_STEP = 1.0f / 240.0f;

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

        last_update_time = current_time;

        if(frame_time > MAX_TIME_STEP)
        {
            float total_frame_time = frame_time;
            while(total_frame_time > 0.0f)
            {
                float dt = std::min(total_frame_time, MAX_TIME_STEP);
                total_frame_time -= MAX_TIME_STEP;
                Update(dt);
            }
        }
        else
        {
            Update(frame_time);
        }

        Render();

        Input::BeginNewFrame();
        glfwPollEvents();
    }

    glfwTerminate();
}

float Engine::Ticks()
{
    // returns the time in seconds (since glfw was initialized)
    return glfwGetTime();
}

void Engine::Render()
{
    Graphics::Clear();
    Graphics::ClearFrameBuffers();

    Draw();

    Graphics::EndBatch();
    Graphics::FlushBatch();

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