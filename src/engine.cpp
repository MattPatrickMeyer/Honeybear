#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine.h"
#include "graphics.h"

using namespace Honeybear;

uint32_t test_frame_buffer;
uint32_t another_test_frame_buffer;

void Engine::Init(int window_width, int window_height, const std::string& window_title)
{
    Graphics::Init(window_width, window_height, window_title);

    Graphics::LoadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");

    Graphics::LoadTexture("sprites", "res/images/sprites.png");

    Graphics::CreateSprite(1, "sprites", 0,   96,  32, 32);
    Graphics::CreateSprite(2, "sprites", 0,   128, 32, 32);
    Graphics::CreateSprite(3, "sprites", 448, 128, 32, 32);

    test_frame_buffer = Graphics::AddFrameBuffer(window_width, window_height);
    another_test_frame_buffer = Graphics::AddFrameBuffer(window_width / 2, window_height / 2);
}

void Engine::Run()
{
    while(!glfwWindowShouldClose(Graphics::window))
    {
        ProcessInput();

        Render();

        glfwPollEvents();
    }

    glfwTerminate();
}

void Engine::Render()
{
    Graphics::Clear();

    Graphics::ActivateShader("default");

    Graphics::BindTexture("sprites", 0);

    for(int x = 0; x < 60; ++x)
    {
        for(int y = 0; y < 34; ++y)
        {
            Graphics::DrawSprite(Graphics::sprites[1], glm::vec2(x * 32.0f, y * 32.0f), test_frame_buffer);
        }
    }

    Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(0 * 32.0f, 0 * 32.0f), another_test_frame_buffer);
    //Graphics::DrawSprite(Graphics::sprites[3], glm::vec2(6 * 32.0f, 5 * 32.0f), another_test_frame_buffer);

    Graphics::RenderFrameBuffer(test_frame_buffer);
    Graphics::RenderFrameBuffer(another_test_frame_buffer);

    Graphics::SwapBuffers();
}

void Engine::ProcessInput()
{
    if(glfwGetKey(Graphics::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Graphics::window, true);
    }
}