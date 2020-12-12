#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>

struct GLFWwindow;

namespace Honeybear
{
    namespace Input
    {
        enum Key
        {
            KEY_Q,
            KEY_W,
            KEY_E,
            KEY_R,
            KEY_T,
            KEY_Y,
            KEY_U,
            KEY_I,
            KEY_O,
            KEY_P,
            KEY_A,
            KEY_S,
            KEY_D,
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_Z,
            KEY_X,
            KEY_C,
            KEY_V,
            KEY_B,
            KEY_N,
            KEY_M,
            KEY_LEFT_ARROW,
            KEY_RIGHT_ARROW,
            KEY_UP_ARROW,
            KEY_DOWN_ARROW,
            KEY_GRAVE_ACCENT,
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_MINUS,
            KEY_EQUAL,
            KEY_ESC,
            KEY_SPACE,
            KEY_LEFT_SHIFT,
            KEY_RIGHT_SHIFT,
            KEY_LEFT_CTRL,
            KEY_RIGHT_CTRL,
            KEY_LEFT_ALT,
            KEY_RIGHT_ALT,
            KEY_TAB,
            KEY_CAPS_LOCK,
            KEY_BACKSPACE,
            KEY_BACKSLASH,
            KEY_ENTER,
            KEY_F1,
            KEY_F2,
            KEY_F3,
            KEY_F4,
            KEY_F5,
            KEY_F6,
            KEY_F7,
            KEY_F8,
            KEY_F9,
            KEY_F10,
            KEY_F11,
            KEY_F12,
            KEY_PRINT_SCREEN,
            KEY_SCROLL_LOCK,
            KEY_PAUSE,
            KEY_INSERT,
            KEY_HOME,
            KEY_PAGE_UP,
            KEY_PAGE_DOWN,
            KEY_DELETE,
            KEY_END
        };

        extern std::unordered_map<Key, bool> held_keys;
        extern std::unordered_map<Key, bool> pressed_keys;
        extern std::unordered_map<Key, bool> released_keys;

        void Init();
        void BeginNewFrame();
        void KeyCallback(GLFWwindow* window, int glfw_key, int scancode, int action, int mods);

        bool WasKeyPressed(Key key);
        bool WasKeyReleased(Key key);
        bool IsKeyHeld(Key key);

        void CursorWindowPosition(GLFWwindow* window, double* x_pos, double* y_pos);
        void CursorGamePosition(GLFWwindow* window, double* x_pos, double* y_pos);
    };
};

#endif