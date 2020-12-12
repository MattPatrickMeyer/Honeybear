#include "engine.h"
#include "input.h"
#include "graphics.h"

using namespace Honeybear;

std::unordered_map<Input::Key, bool> Input::held_keys;
std::unordered_map<Input::Key, bool> Input::pressed_keys;
std::unordered_map<Input::Key, bool> Input::released_keys;

std::unordered_map<int, Input::Key> glfw_key_map =
{
    { GLFW_KEY_ESCAPE, Input::ESC_KEY },
    { GLFW_KEY_Q,      Input::Q_KEY },
    { GLFW_KEY_W,      Input::W_KEY },
    { GLFW_KEY_E,      Input::E_KEY },
    { GLFW_KEY_R,      Input::R_KEY },
    { GLFW_KEY_T,      Input::T_KEY },
    { GLFW_KEY_Y,      Input::Y_KEY },
    { GLFW_KEY_U,      Input::U_KEY },
    { GLFW_KEY_I,      Input::I_KEY },
    { GLFW_KEY_O,      Input::O_KEY },
    { GLFW_KEY_P,      Input::P_KEY },
    { GLFW_KEY_A,      Input::A_KEY },
    { GLFW_KEY_S,      Input::S_KEY },
    { GLFW_KEY_D,      Input::D_KEY },
    { GLFW_KEY_F,      Input::F_KEY },
    { GLFW_KEY_G,      Input::G_KEY },
    { GLFW_KEY_H,      Input::H_KEY },
    { GLFW_KEY_J,      Input::J_KEY },
    { GLFW_KEY_K,      Input::K_KEY },
    { GLFW_KEY_L,      Input::L_KEY },
    { GLFW_KEY_Z,      Input::Z_KEY },
    { GLFW_KEY_X,      Input::X_KEY },
    { GLFW_KEY_C,      Input::C_KEY },
    { GLFW_KEY_V,      Input::V_KEY },
    { GLFW_KEY_B,      Input::B_KEY },
    { GLFW_KEY_N,      Input::N_KEY },
    { GLFW_KEY_M,      Input::M_KEY }
};

void Input::Init()
{
    glfwSetKeyCallback(Graphics::window, KeyCallback);
}

void Input::BeginNewFrame()
{
    // clear the pressed and released keys. these would have been queried by now
    pressed_keys.clear();
    released_keys.clear();
}

void Input::CursorWindowPosition(GLFWwindow* window, double* x_pos, double* y_pos)
{
    glfwGetCursorPos(window, x_pos, y_pos);
}

void Input::CursorGamePosition(GLFWwindow* window, double* x_pos, double* y_pos)
{
    glfwGetCursorPos(window, x_pos, y_pos);

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    float scale_x = Honeybear::game_width / window_width;
    float scale_y = Honeybear::game_height / window_height;

    *x_pos *= scale_x;
    *y_pos *= scale_y;
}

void Input::KeyCallback(GLFWwindow* window, int glfw_key, int scancode, int action, int mods)
{
    // just return if we don't know about this key
    std::unordered_map<int, Key>::iterator it = glfw_key_map.find(glfw_key);
    if(it == glfw_key_map.end())
    {
        return;
    }

    Input::Key key = it->second;

    if(action == GLFW_PRESS)
    {
        pressed_keys[key] = true;
        held_keys[key] = true;
    }
    else if(action == GLFW_RELEASE)
    {
        released_keys[key] = true;
        held_keys[key] = false;
    }
}

bool Input::WasKeyPressed(Key key)
{
    return pressed_keys[key];
}

bool Input::WasKeyReleased(Key key)
{
    return released_keys[key];
}

bool Input::IsKeyHeld(Key key)
{
    return held_keys[key];
}