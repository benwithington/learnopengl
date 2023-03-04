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
unsigned int loadTexture(std::string path);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

    State()
        : screenWidth(1280),
          screenHeight(720),
          title("OpenGL"),
          camera(Camera{glm::vec3(0.0f, 0.0f, 3.0f)}),
          lastX(screenWidth / 2.0f),
          lastY(screenHeight / 2.0f),
          firstMouse(true),
          deltaTime(0.0f),
          lastFrame(0.0f) {}
};

int main() {
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

    State state{};

    /* Create a windowed mode window and its OpenGL context */
    std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow)*> window{
        glfwCreateWindow(state.screenWidth, state.screenHeight,
                         state.title.c_str(), nullptr, nullptr),
        glfwDestroyWindow};

    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetWindowUserPointer(window.get(), &state);

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
    learning::Shader default_shader{"shaders/default.vert",
                                    "shaders/default.frag"};
    learning::Shader screenShader{"shaders/screen.vert", "shaders/screen.frag"};
    learning::Shader assimpShader{"shaders/default.vert",
                                  "shaders/assimp.frag"};

    // Models
    // learning::Model backpack("res/models/backpack/backpack.obj");

    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

    float planeVertices[] = {
        // positions          // texture Coords
        5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
        0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

        5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
        0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

    float quadVertices[] = {// vertex attributes for a quad that fills the
                            // entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                            0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                            1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    // Load Textures
    unsigned int cubeTexture = loadTexture("res/textures/container.jpg");
    unsigned int metalTexture = loadTexture("res/textures/metal.png");

    // shader configuration
    // --------------------
    default_shader.use();
    default_shader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Framebuffers
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    int square = 512;

    // generate texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state.screenWidth,
                 state.screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           textureColorbuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          state.screenWidth, state.screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach it to currently bound framebuffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.get())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        state.deltaTime = currentFrame - state.lastFrame;
        state.lastFrame = currentFrame;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();

        processInput(window.get());

        // render
        // ------
        // bind to framebuffer and draw scene as we normally would to color
        // texture
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);  // enable depth testing (is disabled for
                                  // rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        default_shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = state.camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(state.camera.Zoom),
                             static_cast<float>(state.screenWidth) /
                                 static_cast<float>(state.screenHeight),
                             0.1f, 100.0f);
        default_shader.setMat4("view", view);
        default_shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        default_shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        default_shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        default_shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // now bind back to default framebuffer and draw a quad plane with the
        // attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);  // disable depth test so screen-space quad
                                   // isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(
            1.0f, 1.0f, 1.0f,
            1.0f);  // set clear color to white (not really necessary actually,
                    // since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D,
                      textureColorbuffer);  // use the color attachment texture
                                            // as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // IMGUI
        ImGui::Begin("Test");
        {
            GLint v[4];
            glGetIntegerv(GL_VIEWPORT, v);

            float tempSquare = static_cast<float>(square);

            // ImGui::SetWindowSize(ImVec2(tempSquare, tempSquare));
            // ImGui::SetWindowPos(ImVec2(100, 100));

            glViewport(0, 0, square, square);

            // Calculate the texture coordinates with flipped Y-axis
            ImVec2 uv0 = ImVec2(0.0f, 1.0f);
            ImVec2 uv1 = ImVec2(1.0f, 0.0f);

            ImGui::Image((void*)(intptr_t)textureColorbuffer,
                         ImVec2(tempSquare, tempSquare), uv0, uv1);

            glViewport(0, 0, v[2], v[3]);
        }
        ImGui::End();

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

    // Delete
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);

    glfwTerminate();
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow* window) {
    State* state = static_cast<State*>(glfwGetWindowUserPointer(window));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        state->camera.ProcessKeyboard(FORWARD, state->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        state->camera.ProcessKeyboard(BACKWARD, state->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        state->camera.ProcessKeyboard(LEFT, state->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        state->camera.ProcessKeyboard(RIGHT, state->deltaTime);
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
unsigned int loadTexture(std::string path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
        stbi_load(path.c_str(), &width, &height, &nrComponents, 0),
        stbi_image_free);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data.get());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    return textureID;
}