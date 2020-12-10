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
            ESC_KEY,
            Q_KEY,
            W_KEY,
            E_KEY,
            R_KEY,
            T_KEY,
            Y_KEY,
            U_KEY,
            I_KEY,
            O_KEY,
            P_KEY,
            A_KEY,
            S_KEY,
            D_KEY,
            F_KEY,
            G_KEY,
            H_KEY,
            J_KEY,
            K_KEY,
            L_KEY,
            Z_KEY,
            X_KEY,
            C_KEY,
            V_KEY,
            B_KEY,
            N_KEY,
            M_KEY
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