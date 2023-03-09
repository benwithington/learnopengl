#include "window.h"

#include <cstdlib>  //for exit()
#include <glm/glm.hpp>
#include <iostream>

namespace personal::renderer::utility {

State::State(int width, int height, std::string title)
    : screenWidth(width),
      screenHeight(height),
      title(title),
      camera(Camera{glm::vec3(0.0f, 0.0f, 5.0f)}),
      lastX(screenWidth / 2.0f),
      lastY(screenHeight / 2.0f),
      firstMouse(true),
      deltaTime(0.0f),
      lastFrame(0.0f),
      showDepth(false) {}

Window::Window(int width, int height, std::string title, GLFWmonitor* monitor,
               GLFWwindow* share) {
    if (!glfwInit()) {
        std::cout << "Failed to initialise GLFW\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title.c_str(), monitor, share);

    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    state = State{width, height, title};
    glfwSetWindowUserPointer(window, &state);

    makeContextCurrent();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window() { glfwDestroyWindow(window); }

void Window::makeContextCurrent() { glfwMakeContextCurrent(window); }

GLFWwindow* Window::get() { return window; }

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::processInput() {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        state.camera.ProcessKeyboard(utility::Camera_Movement::FORWARD,
                                     state.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        state.camera.ProcessKeyboard(utility::Camera_Movement::BACKWARD,
                                     state.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        state.camera.ProcessKeyboard(utility::Camera_Movement::LEFT,
                                     state.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        state.camera.ProcessKeyboard(utility::Camera_Movement::RIGHT,
                                     state.deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));
    state->screenWidth = width;
    state->screenHeight = height;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    // Currently unused
    (void)scancode;
    (void)mods;

    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool isMouseDisabled = true;

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        if (isMouseDisabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isMouseDisabled = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isMouseDisabled = true;
        }
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        GLint mode{};
        glGetIntegerv(GL_POLYGON_MODE, &mode);
        if (mode == GL_FILL)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        state->showDepth = !state->showDepth;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));

    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (state->firstMouse) {
        state->lastX = x;
        state->lastY = y;
        state->firstMouse = false;
    }

    float xoffset = x - state->lastX;
    float yoffset = state->lastY - y;

    state->lastX = x;
    state->lastY = y;

    state->camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));
    state->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
}  // namespace personal::renderer::utility