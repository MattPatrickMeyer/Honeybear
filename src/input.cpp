#include "engine.h"
#include "input.h"
#include "graphics.h"

using namespace Honeybear;

std::unordered_map<Input::Key, bool> Input::held_keys;
std::unordered_map<Input::Key, bool> Input::pressed_keys;
std::unordered_map<Input::Key, bool> Input::released_keys;
std::unordered_map<Input::MouseButton, bool> Input::held_mouse_buttons;
std::unordered_map<Input::MouseButton, bool> Input::pressed_mouse_buttons;
std::unordered_map<Input::MouseButton, bool> Input::released_mouse_buttons;
double Input::x_scroll_offset;
double Input::y_scroll_offset;

std::unordered_map<int, Input::Key> glfw_key_map =
{
    { GLFW_KEY_ESCAPE,        Input::KEY_ESC },
    { GLFW_KEY_Q,             Input::KEY_Q },
    { GLFW_KEY_W,             Input::KEY_W },
    { GLFW_KEY_E,             Input::KEY_E },
    { GLFW_KEY_R,             Input::KEY_R },
    { GLFW_KEY_T,             Input::KEY_T },
    { GLFW_KEY_Y,             Input::KEY_Y },
    { GLFW_KEY_U,             Input::KEY_U },
    { GLFW_KEY_I,             Input::KEY_I },
    { GLFW_KEY_O,             Input::KEY_O },
    { GLFW_KEY_P,             Input::KEY_P },
    { GLFW_KEY_A,             Input::KEY_A },
    { GLFW_KEY_S,             Input::KEY_S },
    { GLFW_KEY_D,             Input::KEY_D },
    { GLFW_KEY_F,             Input::KEY_F },
    { GLFW_KEY_G,             Input::KEY_G },
    { GLFW_KEY_H,             Input::KEY_H },
    { GLFW_KEY_J,             Input::KEY_J },
    { GLFW_KEY_K,             Input::KEY_K },
    { GLFW_KEY_L,             Input::KEY_L },
    { GLFW_KEY_Z,             Input::KEY_Z },
    { GLFW_KEY_X,             Input::KEY_X },
    { GLFW_KEY_C,             Input::KEY_C },
    { GLFW_KEY_V,             Input::KEY_V },
    { GLFW_KEY_B,             Input::KEY_B },
    { GLFW_KEY_N,             Input::KEY_N },
    { GLFW_KEY_M,             Input::KEY_M },
    { GLFW_KEY_LEFT,          Input::KEY_LEFT_ARROW },
    { GLFW_KEY_RIGHT,         Input::KEY_RIGHT_ARROW },
    { GLFW_KEY_UP,            Input::KEY_UP_ARROW },
    { GLFW_KEY_DOWN,          Input::KEY_DOWN_ARROW },
    { GLFW_KEY_GRAVE_ACCENT,  Input::KEY_GRAVE_ACCENT },
    { GLFW_KEY_1,             Input::KEY_1 },
    { GLFW_KEY_2,             Input::KEY_2 },
    { GLFW_KEY_3,             Input::KEY_3 },
    { GLFW_KEY_4,             Input::KEY_4 },
    { GLFW_KEY_5,             Input::KEY_5 },
    { GLFW_KEY_6,             Input::KEY_6 },
    { GLFW_KEY_7,             Input::KEY_7 },
    { GLFW_KEY_8,             Input::KEY_8 },
    { GLFW_KEY_9,             Input::KEY_9 },
    { GLFW_KEY_MINUS,         Input::KEY_MINUS },
    { GLFW_KEY_EQUAL,         Input::KEY_EQUAL },
    { GLFW_KEY_SPACE,         Input::KEY_SPACE },
    { GLFW_KEY_LEFT_SHIFT,    Input::KEY_LEFT_SHIFT },
    { GLFW_KEY_RIGHT_SHIFT,   Input::KEY_RIGHT_SHIFT },
    { GLFW_KEY_LEFT_CONTROL,  Input::KEY_LEFT_CTRL },
    { GLFW_KEY_RIGHT_CONTROL, Input::KEY_RIGHT_CTRL },
    { GLFW_KEY_LEFT_ALT,      Input::KEY_LEFT_ALT },
    { GLFW_KEY_RIGHT_ALT,     Input::KEY_RIGHT_ALT },
    { GLFW_KEY_TAB,           Input::KEY_TAB },
    { GLFW_KEY_CAPS_LOCK,     Input::KEY_CAPS_LOCK },
    { GLFW_KEY_BACKSPACE,     Input::KEY_BACKSPACE },
    { GLFW_KEY_BACKSLASH,     Input::KEY_BACKSLASH },
    { GLFW_KEY_ENTER,         Input::KEY_ENTER },
    { GLFW_KEY_F1,            Input::KEY_F1 },
    { GLFW_KEY_F2,            Input::KEY_F2 },
    { GLFW_KEY_F3,            Input::KEY_F3 },
    { GLFW_KEY_F4,            Input::KEY_F4 },
    { GLFW_KEY_F5,            Input::KEY_F5 },
    { GLFW_KEY_F6,            Input::KEY_F6 },
    { GLFW_KEY_F7,            Input::KEY_F7 },
    { GLFW_KEY_F8,            Input::KEY_F8 },
    { GLFW_KEY_F9,            Input::KEY_F9 },
    { GLFW_KEY_F10,           Input::KEY_F10 },
    { GLFW_KEY_F11,           Input::KEY_F11 },
    { GLFW_KEY_F12,           Input::KEY_F12 },
    { GLFW_KEY_PRINT_SCREEN,  Input::KEY_PRINT_SCREEN },
    { GLFW_KEY_SCROLL_LOCK,   Input::KEY_SCROLL_LOCK },
    { GLFW_KEY_PAUSE,         Input::KEY_PAUSE },
    { GLFW_KEY_INSERT,        Input::KEY_INSERT },
    { GLFW_KEY_HOME,          Input::KEY_HOME },
    { GLFW_KEY_PAGE_UP,       Input::KEY_PAGE_UP },
    { GLFW_KEY_PAGE_DOWN,     Input::KEY_PAGE_DOWN },
    { GLFW_KEY_DELETE,        Input::KEY_DELETE },
    { GLFW_KEY_END,           Input::KEY_END }
};

std::unordered_map<int, Input::MouseButton> glfw_mouse_button_map =
{
    { GLFW_MOUSE_BUTTON_LEFT,   Input::MOUSE_BUTTON_LEFT },
    { GLFW_MOUSE_BUTTON_RIGHT,  Input::MOUSE_BUTTON_RIGHT },
    { GLFW_MOUSE_BUTTON_MIDDLE, Input::MOUSE_BUTTON_MIDDLE }
};

void Input::Init()
{
    glfwSetKeyCallback(Graphics::window, KeyCallback);
    glfwSetMouseButtonCallback(Graphics::window, MouseButtonCallback);
    glfwSetScrollCallback(Graphics::window, MouseScrollCallback);

    x_scroll_offset = 0.0f;
    y_scroll_offset = 0.0f;
}

void Input::BeginNewFrame()
{
    // clear the pressed and released keys. these would have been queried by now
    pressed_keys.clear();
    released_keys.clear();
    pressed_mouse_buttons.clear();
    released_mouse_buttons.clear();
    x_scroll_offset = 0.0f;
    y_scroll_offset = 0.0f;
}

void Input::CursorWindowPosition(float* x_pos, float* y_pos)
{
    double x, y;
    glfwGetCursorPos(Graphics::window, &x, &y);
    *x_pos = x;
    *y_pos = y;
}

void Input::CursorGamePosition(float* x_pos, float* y_pos)
{
    double x, y;
    glfwGetCursorPos(Graphics::window, &x, &y);

    int window_width, window_height;
    glfwGetWindowSize(Graphics::window, &window_width, &window_height);

    float scale_x = Honeybear::game_width / window_width;
    float scale_y = Honeybear::game_height / window_height;

    *x_pos = x * scale_x;
    *y_pos = y * scale_y;
}

void Input::CursorWindowPosition(Vec2* pos)
{
    CursorWindowPosition(&pos->x, &pos->y);
}

void Input::CursorGamePosition(Vec2* pos)
{
    CursorGamePosition(&pos->x, &pos->y);
}

void Input::KeyCallback(GLFWwindow* window, int glfw_key, int scancode, int action, int mods)
{
    // just return if we don't know about this key
    std::unordered_map<int, Input::Key>::iterator it = glfw_key_map.find(glfw_key);
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

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // just return if we don't know about this mouse button
    std::unordered_map<int, Input::MouseButton>::iterator it = glfw_mouse_button_map.find(button);
    if(it == glfw_mouse_button_map.end())
    {
        return;
    }

    Input::MouseButton mouse_button = it->second;

    if(action == GLFW_PRESS)
    {
        pressed_mouse_buttons[mouse_button] = true;
        held_mouse_buttons[mouse_button] = true;
    }
    else if(action == GLFW_RELEASE)
    {
        released_mouse_buttons[mouse_button] = true;
        held_mouse_buttons[mouse_button] = false;
    }
}

void Input::MouseScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    x_scroll_offset = x_offset;
    y_scroll_offset = y_offset;
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

bool Input::WasMouseButtonPressed(MouseButton mouse_button)
{
    return pressed_mouse_buttons[mouse_button];
}

bool Input::WasMouseButtonReleased(MouseButton mouse_button)
{
    return released_mouse_buttons[mouse_button];
}

bool Input::IsMouseButtonHeld(MouseButton mouse_button)
{
    return held_mouse_buttons[mouse_button];
}

bool Input::MouseScrolledUp()
{
    return y_scroll_offset > 0.0f;
}

bool Input::MouseScrolledDown()
{
    return y_scroll_offset < 0.0f;
}