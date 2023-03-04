#ifndef WINDOW_H
#define WINDOW_H

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "camera.h"
// clang-format on

namespace personal::renderer::utility {

struct State {
    int screenWidth;
    int screenHeight;
    std::string title;

    Camera camera;
    float lastX;
    float lastY;
    bool firstMouse;

    float deltaTime;
    float lastFrame;

    State(int width, int height, std::string title);
};

class Window {
   public:
    int width;
    int height;
    std::string title;
    State state{width, height, title};

    Window(int width = 1280, int height = 720, std::string title = "OpenGL",
           GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
    ~Window();
    void makeContextCurrent();
    GLFWwindow* get();
    bool shouldClose();
    void processInput();

   private:
    GLFWwindow* window;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
}  // namespace personal::renderer::utility

#endif  // WINDOW_H