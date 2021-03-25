#ifndef ENGINE_H
#define ENGINE_H

#define FPS_RECORD_COUNT 100

#include <string>

namespace Honeybear
{
    extern float game_width;
    extern float game_height;
    extern float game_speed;

    namespace Engine
    {
        extern int average_fps;
        extern float last_frame_time;
        extern double fixed_time_step;
        extern double total_elapsed_time;

        typedef void (*draw_function)(void);
        typedef void (*update_function)(const float dt);
        typedef void (*begin_frame_function)(void);
        typedef void (*update_fixed_function)(const double dt);
        typedef void (*interpolate_state_function)(const double t);

        extern draw_function draw_func;
        extern update_function update_func;
        extern begin_frame_function begin_frame_func;
        extern update_fixed_function update_fixed_func;
        extern interpolate_state_function interpolate_state_func;

        void Init(int window_width, int window_height, const std::string& window_title);
        void Run();
        void Render();
        void Quit();

        double Ticks();

        void SetDrawCallback(draw_function func);
        void SetUpdateCallback(update_function func);
        void SetBeginFrameCallback(begin_frame_function func);
        void SetUpdateFixedCallback(update_fixed_function func);
        void SetInterpolateStateCallback(interpolate_state_function func);

        void SetGameSize(const float w, const float h);
        void SetFixedTimeStep(const float value);
    };
};

#endif