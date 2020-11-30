#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine.h"
#include "graphics.h"

using namespace Honeybear;

void Engine::Init(int window_width, int window_height, const std::string& window_title)
{
    Graphics::Init(window_width, window_height, window_title);
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

    Draw();

    Graphics::SwapBuffers();
}

void Engine::ProcessInput()
{
    if(glfwGetKey(Graphics::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Graphics::window, true);
    }
}