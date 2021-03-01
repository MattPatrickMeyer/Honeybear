#include <thread>
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

int Honeybear::Engine::average_fps;
float Honeybear::Engine::last_frame_time;
double Honeybear::Engine::fixed_time_step;
double Honeybear::Engine::total_elapsed_time = 0.0f;

Engine::draw_function Engine::draw_func;
Engine::update_function Engine::update_func;
Engine::begin_frame_function Engine::begin_frame_func;
Engine::update_fixed_function Engine::update_fixed_func;
Engine::interpolate_state_function Engine::interpolate_state_func;
Engine::handle_input_function Engine::handle_input_func;

namespace
{
    const float MAX_TIME_STEP = 1.0f / 240.0f;
    const double DEFAULT_FIXED_TIME_STEP = 1.0 / 240.0f;

    float fps_records[FPS_RECORD_COUNT];
    size_t fps_record_index = 0;
}

void Engine::Init(int window_width, int window_height, const std::string& window_title)
{
    game_width = window_width;
    game_height = window_height;
    fixed_time_step = DEFAULT_FIXED_TIME_STEP;
    Graphics::Init(window_width, window_height, window_title);
    Input::Init();
}

void Engine::RunFixed()
{
    double elapsed_time = 0.0f;
    double current_time = Ticks();
    double accumulator = 0.0f;

    while(!glfwWindowShouldClose(Graphics::window))
    {
        Input::BeginNewFrame();
        glfwPollEvents();

        ProcessInput();

        begin_frame_func();

        double new_time = Ticks();
        double frame_time = (new_time - current_time) * game_speed;
        double actual_frame_time = new_time - current_time;

        if(frame_time > 0.25) // to avoid spiral of death...
        {
            frame_time = 0.25;
        }

        current_time = new_time;
        accumulator += frame_time;

        while(accumulator >= fixed_time_step)
        {
            //UpdateFixed(fixed_time_step);
            update_fixed_func(fixed_time_step);
            elapsed_time += fixed_time_step;
            total_elapsed_time = elapsed_time;
            accumulator -= fixed_time_step;
        }

        // the left over time
        const double alpha = accumulator / fixed_time_step;

        // interpolate the game from previous to current state based on alpha value
        //InterpolateState(alpha);
        interpolate_state_func(alpha);

        Render();

        // -- calc average frame rate --
        fps_records[fps_record_index] = 1.0f / actual_frame_time;
        fps_record_index = (fps_record_index + 1) % 100;

        float total = 0.0f;
        for(size_t i = 0; i < FPS_RECORD_COUNT; ++i)
        {
            total += fps_records[i];
        }
        average_fps = int(total / FPS_RECORD_COUNT);
        // -----------------------------
    }

    glfwTerminate();
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
                //Update(dt);
                update_func(dt);
            }
        }
        else
        {
            float dt = frame_time * game_speed;
            //Update(dt);
            update_func(dt);
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

    //Draw();
    draw_func();

    Graphics::SwapBuffers();
}

void Engine::ProcessInput()
{
    //HandleInput();
    handle_input_func();
}

void Engine::Quit()
{
    // todo: other stuff here
    glfwSetWindowShouldClose(Graphics::window, true);
}

void Engine::SetGameSize(const float w, const float h)
{
    game_width = w;
    game_height = h;
}

void Engine::SetFixedTimeStep(const float value)
{
    fixed_time_step = value;
}

void Engine::SetDrawCallback(draw_function func)
{
    draw_func = func;
}

void Engine::SetUpdateCallback(update_function func)
{
    update_func = func;
}

void Engine::SetBeginFrameCallback(begin_frame_function func)
{
    begin_frame_func = func;
}

void Engine::SetUpdateFixedCallback(update_fixed_function func)
{
    update_fixed_func = func;
}

void Engine::SetInterpolateStateCallback(interpolate_state_function func)
{
    interpolate_state_func = func;
}

void Engine::SetHandleInputCallback(handle_input_function func)
{
    handle_input_func = func;
}