// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "window.h"
#include "texture.h"

// clang-format on

using namespace personal::renderer;

int main() {
    utility::Window window{};

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // imgui setup
    // -----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    stbi_set_flip_vertically_on_load(true);

    utility::Shader asteroidShader("shaders/asteroid.vert",
                                   "shaders/asteroid.frag");
    utility::Shader planetShader("shaders/planet.vert", "shaders/planet.frag");
    utility::Shader baseShader("shaders/default.vert", "shaders/default.frag");
    utility::Shader singleColour("shaders/default.vert",
                                 "shaders/single_colour.frag");

    utility::AssimpModel rock("res/models/rock/rock.obj");
    utility::AssimpModel planet("res/models/planet/planet.obj");
    utility::AssimpModel cube("res/models/cube/cube.obj");

    [[maybe_unused]] unsigned int containerTexture{
        utility::loadTexture("res/textures/container.jpg")};

    singleColour.use();
    singleColour.setVec3("colour", glm::vec3(0.0f, 1.0f, 0.0f));

    // render loop
    // -----------
    while (!window.shouldClose()) {
        // Calc delta time
        // ---------------
        float currentFrame = static_cast<float>(glfwGetTime());
        window.state.deltaTime = currentFrame - window.state.lastFrame;
        window.state.lastFrame = currentFrame;

        // imgui frame init
        // ----------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Process user input
        // ------------------
        window.processInput();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = window.state.camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(window.state.camera.Zoom),
                             static_cast<float>(window.state.screenWidth) /
                                 static_cast<float>(window.state.screenHeight),
                             0.1f, 1000.0f);

        singleColour.use();
        singleColour.setMat4("model", model);
        singleColour.setMat4("view", view);
        singleColour.setMat4("projection", projection);
        cube.draw(singleColour);

        // ImGui end frame
        // ---------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // shutdown imgui
    // --------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}