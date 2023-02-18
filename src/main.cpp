// clang-format off

//Standard Library
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <vector>

//External Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//Personal Headers
#include "shader.h"
#include "camera.h"
#include "model.h"
// clang-format on

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const int screen_width{1280};
const int screen_height{720};

Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse{true};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    // Assimp::Importer importer;

    /* Initialize the library */
    if (!glfwInit()) {
        std::cout << "Failed to initialise GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // for mac
#endif

    /* Create a windowed mode window and its OpenGL context */
    std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow)*> window{
        glfwCreateWindow(screen_width, screen_height, "OpenGL", nullptr,
                         nullptr),
        glfwDestroyWindow};

    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window.get());
    glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
    glfwSetKeyCallback(window.get(), key_callback);
    glfwSetCursorPosCallback(window.get(), mouse_callback);
    glfwSetScrollCallback(window.get(), scroll_callback);

    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading
    // model).
    stbi_set_flip_vertically_on_load(true);

    // IMGUI SETUP
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Global opengl state
    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader default_shader{"shaders/default.vert", "shaders/default.frag"};

    // Models
    learning::Model backpack("res/models/backpack/backpack.obj");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.get())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        processInput(window.get());

        /* Render here */
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        default_shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom),
            (float)screen_width / (float)screen_height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        default_shader.setMat4("projection", projection);
        default_shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(
            model,
            glm::vec3(
                0.0f, 0.0f,
                0.0f));  // translate it down so it's at the center of the scene
        model = glm::scale(
            model,
            glm::vec3(
                1.0f, 1.0f,
                1.0f));  // it's a bit too big for our scene, so scale it down
        default_shader.setMat4("model", model);
        backpack.draw(default_shader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window.get());

        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    // Currently unused
    (void)scancode;
    (void)mods;

    static bool isMouseDisabled = true;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        if (isMouseDisabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isMouseDisabled = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isMouseDisabled = true;
        }
    }
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;

    lastX = x;
    lastY = y;

    camera.processMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    camera.processMouseScroll(static_cast<float>(yoffset));
}
