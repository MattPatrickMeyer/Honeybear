#ifndef ENGINE_H
#define ENGINE_H

#include <string>

namespace Honeybear
{
    extern float game_width;
    extern float game_height;

    struct Engine
    {
        void Init(int window_width, int window_height, const std::string& window_title);
        void Run();
        void Render();
        void ProcessInput();

        float Ticks();

        virtual void Draw() = 0;
        virtual void Update(const float dt) = 0;

        protected:
            void SetGameSize(const float w, const float h);
    };
};

#endif