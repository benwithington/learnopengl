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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    /*
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    */

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

    // build and compile shaders
    // -------------------------
    utility::Shader assimpShader{"shaders/assimp.vert", "shaders/assimp.frag",
                                 "shaders/assimp.geom"};
    utility::Shader assimpShaderWithoutGeom{"shaders/assimp.vert",
                                            "shaders/assimp.frag"};
    utility::Shader geomShader("shaders/geometry.vert", "shaders/geometry.frag",
                               "shaders/geometry.geom");
    utility::Shader skyboxShader{"shaders/skybox.vert", "shaders/skybox.frag"};
    utility::Shader normalsShader{"shaders/show_normals.vert",
                                  "shaders/show_normals.frag",
                                  "shaders/show_normals.geom"};
    utility::Shader instancingShader{"shaders/instancing.vert",
                                     "shaders/instancing.frag"};

    // Load CubeMaps
    // -------------
    std::vector<std::string> faces{"right.jpg",  "left.jpg",  "top.jpg",
                                   "bottom.jpg", "front.jpg", "back.jpg"};

    [[maybe_unused]] unsigned int skyboxCubeMap{
        utility::loadCubemap("res/textures/skybox/", faces)};

    // Load Textures
    // -------------
    stbi_set_flip_vertically_on_load(true);

    [[maybe_unused]] unsigned int cubeTexture{
        utility::loadTexture("res/textures/container.jpg")};

    // Model vertices data
    // -------------------
    std::vector<float> cubePositions{
        // Back face
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
        // Front face
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
        // Left face
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f,
        // Top face
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f};

    std::vector<float> cubeTexCoords{
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

    std::vector<float> cubeNormals{
        0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,
        0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,

        0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,
        -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

        1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,
        0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,

        0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f};

    // Load Raw Models
    // ---------------
    utility::RawModel cube{cubePositions, cubeTexCoords, cubeNormals};

    // Load Assimp Models
    // -----------
    // utility::AssimpModel backpack{"res/models/backpack/backpack.obj"};

    // pre-frame shader config
    // -----------------------
    assimpShader.use();
    assimpShader.setInt("texture_diffuse1", 0);
    assimpShader.setUniformBlockBinding("matrices", 0);

    assimpShaderWithoutGeom.use();
    assimpShaderWithoutGeom.setInt("texture_diffuse1", 0);
    assimpShaderWithoutGeom.setUniformBlockBinding("matrices", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // uniform buffer objects
    // ----------------------
    unsigned int ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
                 GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> points{
        -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  // top-left
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // top-right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f,  // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f   // bottom-left
    };
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    std::vector<float> quadVerts{
        -0.05f, 0.05f, 1.0f,   0.0f,   0.0f, 0.05f, -0.05f, 0.0f,
        1.0f,   0.0f,  -0.05f, -0.05f, 0.0f, 0.0f,  1.0f,

        -0.05f, 0.05f, 1.0f,   0.0f,   0.0f, 0.05f, -0.05f, 0.0f,
        1.0f,   0.0f,  0.05f,  0.05f,  0.0f, 1.0f,  1.0f};

    unsigned int quadvao;
    glGenVertexArrays(1, &quadvao);
    glBindVertexArray(quadvao);

    unsigned int quadvbo;
    glGenBuffers(1, &quadvbo);
    glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
    glBufferData(GL_ARRAY_BUFFER, quadVerts.size() * sizeof(float),
                 &quadVerts[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    std::vector<glm::vec2> translations;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            translations.emplace_back(
                glm::vec2((static_cast<float>(x) / 10.0f) + offset,
                          (static_cast<float>(y) / 10.0f) + offset));
        }
    }

    unsigned int instancevbo;
    glGenBuffers(1, &instancevbo);
    glBindBuffer(GL_ARRAY_BUFFER, instancevbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * translations.size(),
                 &translations[0], GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);

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

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = window.state.camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f),
                             static_cast<float>(window.state.screenWidth) /
                                 static_cast<float>(window.state.screenHeight),
                             0.1f, 100.0f);

        instancingShader.use();
        glBindVertexArray(quadvao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        // ImGui::ShowDemoWindow();

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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // shutdown imgui
    // --------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}