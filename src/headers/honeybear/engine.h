#ifndef ENGINE_H
#define ENGINE_H

#define FPS_RECORD_COUNT 100

#include <string>

namespace Honeybear
{
    extern float game_width;
    extern float game_height;
    extern float game_speed;
    extern double total_elapsed_time;

    struct Engine
    {
        static int average_fps;
        static float last_frame_time;

        void Init(int window_width, int window_height, const std::string& window_title);
        void Run();
        void Render();
        void ProcessInput();

        double Ticks();

        virtual void Draw() = 0;
        virtual void Update(const float dt) = 0;
        //virtual void UpdateFixed() = 0;
        virtual void HandleInput() = 0;

        protected:
            void SetGameSize(const float w, const float h);
    };
};

#endif