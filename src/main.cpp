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
#include "window.h"
#include "texture.h"
// clang-format on

using namespace personal::renderer;

int main() {
    utility::Window window{};

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

    // Shaders
    utility::Shader default_shader{"shaders/default.vert",
                                   "shaders/default.frag"};
    utility::Shader screenShader{"shaders/screen.vert", "shaders/screen.frag"};
    utility::Shader assimpShader{"shaders/default.vert", "shaders/assimp.frag"};
    utility::Shader singleColour{"shaders/default.vert",
                                 "shaders/singleColour.frag"};

    // Models
    // utility::AssimpModel backpack("res/models/backpack/backpack.obj");

    std::vector<float> cubeVertices{
        // positions
        -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
        0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
        0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};

    std::vector<float> cubeTexCoords{0.0f, 0.0f, 1.0f, 0.0f, 01.0f, 1.0f,
                                     1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f,

                                     0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f,
                                     1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f,

                                     1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f,

                                     1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f,

                                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,
                                     1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f,

                                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,
                                     1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};

    std::vector<float> planeVertices{// positions
                                     5.0f,  -0.5f, 5.0f,  -5.0f, -0.5f, 5.0f,
                                     -5.0f, -0.5f, -5.0f, 5.0f,  -0.5f, 5.0f,
                                     -5.0f, -0.5f, -5.0f, 5.0f,  -0.5f, -5.0f};

    std::vector<float> planeTexCoords{2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f,
                                      2.0f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f};

    utility::RawModel cubeModel{cubeVertices, cubeTexCoords};
    utility::RawModel planeModel{planeVertices, planeTexCoords};

    // Load Textures
    unsigned int cubeTexture =
        utility::loadTexture("res/textures/container.jpg");
    unsigned int metalTexture = utility::loadTexture("res/textures/metal.png");

    // shader configuration
    // --------------------
    default_shader.use();
    default_shader.setInt("texture1", 0);
    default_shader.setBool("showDepth", false);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Global opengl state
    glEnable(GL_DEPTH_TEST);
    /*
     * Use to disable writing to the depth buffer, making it read only
     * glDepthMask(GL_FALSE);
     */

    /*
     * Tell opengl when it should pass or discard fragments by setting the
     * comparison operator
     * glDepthFunc(GL_LESS); // Default value
     * Options:
     *      GL_ALWAYS       -   Always passes
     *      GL_NEVER        -   Never passes
     *      GL_LESS         -   Passes if frag depth is less than stored depth
     *      GL_EQUAL        -   Passes if frag depth is equal to stored depth
     *      GL_LEQUAL       -   Passes if frag depth is less than or equal to
     *                          stored depth
     *      GL_GREATER      -   Passes if frag depth is greater than stored
     *                          depth
     *      GL_NOTEQUAL     -   Passes if frag depth is not euqal to stored
     *                          depth
     *      GL_GEQUAL       -   Passes if the frag depth is greater than or
     *                          equal to the stored depth
     */
    glDepthFunc(GL_LESS);  // default values

    /*
     *  Stencil testing
     */
    glEnable(GL_STENCIL_TEST);

    /*
     *  An equivalend to the glDepthMask(GL_FALSE), for the stencil buffer is:
     *  glStencilMask(0x00), this ANDs the stencil values with the bitmask 0x00,
     *  then all stencil values written to the buffer end up as 0s
     *  The default behaviour is the same as setting the bitmask as "0xFF"
     *  glStencilMask(0xFF)
     */
    glStencilMask(0x00);  // each bit ends up as 0, essentially disables writing
                          // to the stencil buffer
    glStencilMask(0xFF);  // each bit is written to the stencil buffer as is

    /*
     *  Stencil functions
     *  There are two functions that can be used to configure opengl stencil
     *  testing
     *  glStencilFunc and glStencilOp
     *
     *  glStencilFunc(GLenum func, GLint ref, GLuint mask)
     *  -   func
     *      -   Sets the stencil test function that determines whether a
     *          fragment passes or is discarded. This test function is applied
     *          to the stored stencil value and the glStencilFunc's ref value.
     *      -   Possible options are:
     *          -   GL_NEVER
     *          -   GL_LESS
     *          -   GL_LEQUAL
     *          -   GL_GREATER
     *          -   GL_GEQUAL
     *          -   GL_EQUAL
     *          -   GL_NOTEQUAL
     *          -   GL_ALWAYS
     *  -   ref
     *      -   specifies the reference value for the stencil test, the stencil
     *          buffer's content is compared against this value.
     *  -   mask
     *      -   Specifies a mask that is ANDed with both the reference value and
     *          the stored stencil value before the test compares them
     *      -   Initially set to all 1s
     *
     *  glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
     *  -   sfail
     *      -   action to take if the stencil test fails
     *  -   dpfail
     *      -   action to take if the stencil test passes, but the depth test
     *          fails.
     *  -   dppass
     *      -   action to take if both stencil and depth test pass.
     *
     *  Parameters for each of the above parameters are:
     *  -   GL_KEEP
     *      -   the currently stored stencil value is kept
     *  -   GL_ZERO
     *      -   the stencil value is set to 0
     *  -   GL_REPLACE
     *      -   the stencil value is replaced with the reference value set with
     *          glStencilFunc
     *  -   GL_INCR
     *      -   Stencil value incremented if lower than maximum value
     *  -   GL_INCR_WRAP
     *      -   Same as GL_INCR, but wraps to 0 if maximum is exceeded
     *  -   GL_DECR
     *      -   Stencil value decremented if higher than minimum value
     *  -   GL_DECR_WRAP
     *      -   Same as GL_DECR, but wraps to the maximum value if lower than
     *          minimum
     *  -   GL_INVERT
     *      -   bitwise inverts the current stencil buffer value
     *
     *  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);  // default values
     */

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    /* Loop until the user closes the window */
    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        window.state.deltaTime = currentFrame - window.state.lastFrame;
        window.state.lastFrame = currentFrame;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        window.processInput();

        // render
        // ------
        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        glStencilMask(0x00);

        default_shader.use();
        default_shader.setBool("showDepth", window.state.showDepth);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = window.state.camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(window.state.camera.Zoom),
                             static_cast<float>(window.state.screenWidth) /
                                 static_cast<float>(window.state.screenHeight),
                             0.1f, 100.0f);
        default_shader.setMat4("view", view);
        default_shader.setMat4("projection", projection);

        // floor

        glBindTexture(GL_TEXTURE_2D, metalTexture);
        default_shader.setMat4("model", glm::mat4(1.0f));
        planeModel.draw(default_shader);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        // glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        default_shader.setMat4("model", model);
        cubeModel.draw(default_shader);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        default_shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        cubeModel.draw(default_shader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        singleColour.use();
        singleColour.setMat4("view", view);
        singleColour.setMat4("projection", projection);
        float scale = 1.1f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        singleColour.setMat4("model", model);
        cubeModel.draw(singleColour);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        singleColour.setMat4("model", model);
        cubeModel.draw(singleColour);

        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));

        /*
        assimpShader.use();
        assimpShader.setMat4("model", model);
        assimpShader.setMat4("view", view);
        assimpShader.setMat4("projection", projection);
        backpack.draw(assimpShader);
        */

        // IMGUI
        ImGui::ShowDemoWindow();

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