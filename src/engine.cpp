#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine.h"
#include "graphics.h"

using namespace Honeybear;

uint32_t frame_buffer_index;

void Engine::Init(int window_width, int window_height, const std::string& window_title)
{
    Graphics::Init(window_width, window_height, window_title);

    Graphics::LoadShader("default", "res/shaders/default.vert", "res/shaders/default.frag");

    Graphics::LoadTexture("sprites", "res/images/sprites.png");

    Graphics::CreateSprite(1, "sprites", 0, 96,  32, 32);
    Graphics::CreateSprite(2, "sprites", 0, 128, 32, 32);

    frame_buffer_index = Graphics::AddFrameBuffer(window_width, window_height);
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

    Graphics::FrameBuffer* frame_buffer = &Graphics::frame_buffers[frame_buffer_index];

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);

    Graphics::ActivateShader("default");
    Graphics::BindTexture("sprites", 0);

    glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f);
    Graphics::SetMatrix4("default", "projection", projection);

    Graphics::BeginBatch();
    for(int x = 0; x < 100; ++x)
    {
        for(int y = 0; y < 100; ++y)
        {
            Graphics::DrawSprite(Graphics::sprites[2], glm::vec2(x * 32.0f, y * 32.0f));
        }
    }
    Graphics::EndBatch();
    Graphics::FlushBatch();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_buffer->tex_colour_buffer);
    glBindVertexArray(frame_buffer->quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    Graphics::SwapBuffers();
}

void Engine::ProcessInput()
{
    if(glfwGetKey(Graphics::window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Graphics::window, true);
    }
}