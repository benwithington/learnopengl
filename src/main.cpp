// clang-format off

//Standard Library
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <future>
#include <thread>
#include <chrono>

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
    utility::Shader defaultShader{"shaders/default.vert",
                                  "shaders/default.frag"};
    utility::Shader screenShader{"shaders/screen.vert", "shaders/screen.frag"};
    utility::Shader assimpShader{"shaders/default.vert", "shaders/assimp.frag"};
    utility::Shader singleColour{"shaders/default.vert",
                                 "shaders/singleColour.frag"};
    utility::Shader blendingShader{"shaders/default.vert",
                                   "shaders/blending.frag"};
    utility::Shader skyboxShader{"shaders/skybox.vert", "shaders/skybox.frag"};
    utility::Shader enivronmentMappingShader{"shaders/environmentmapping.vert",
                                             "shaders/environmentmapping.frag"};

    // Models
    // utility::AssimpModel backpack("res/models/backpack/backpack.obj");

    std::vector<float> cubeVertices{
        // Back face
        -0.5f, -0.5f, -0.5f,  // Bottom-left
        0.5f, 0.5f, -0.5f,    // top-right
        0.5f, -0.5f, -0.5f,   // bottom-right
        0.5f, 0.5f, -0.5f,    // top-right
        -0.5f, -0.5f, -0.5f,  // bottom-left
        -0.5f, 0.5f, -0.5f,   // top-left
        // Front face
        -0.5f, -0.5f, 0.5f,  // bottom-left
        0.5f, -0.5f, 0.5f,   // bottom-right
        0.5f, 0.5f, 0.5f,    // top-right
        0.5f, 0.5f, 0.5f,    // top-right
        -0.5f, 0.5f, 0.5f,   // top-left
        -0.5f, -0.5f, 0.5f,  // bottom-left
        // Left face
        -0.5f, 0.5f, 0.5f,    // top-right
        -0.5f, 0.5f, -0.5f,   // top-left
        -0.5f, -0.5f, -0.5f,  // bottom-left
        -0.5f, -0.5f, -0.5f,  // bottom-left
        -0.5f, -0.5f, 0.5f,   // bottom-right
        -0.5f, 0.5f, 0.5f,    // top-right
                              // Right face
        0.5f, 0.5f, 0.5f,     // top-left
        0.5f, -0.5f, -0.5f,   // bottom-right
        0.5f, 0.5f, -0.5f,    // top-right
        0.5f, -0.5f, -0.5f,   // bottom-right
        0.5f, 0.5f, 0.5f,     // top-left
        0.5f, -0.5f, 0.5f,    // bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f,  // top-right
        0.5f, -0.5f, -0.5f,   // top-left
        0.5f, -0.5f, 0.5f,    // bottom-left
        0.5f, -0.5f, 0.5f,    // bottom-left
        -0.5f, -0.5f, 0.5f,   // bottom-right
        -0.5f, -0.5f, -0.5f,  // top-right
        // Top face
        -0.5f, 0.5f, -0.5f,  // top-left
        0.5f, 0.5f, 0.5f,    // bottom-right
        0.5f, 0.5f, -0.5f,   // top-right
        0.5f, 0.5f, 0.5f,    // bottom-right
        -0.5f, 0.5f, -0.5f,  // top-left
        -0.5f, 0.5f, 0.5f    // bottom-left
    };
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

    std::vector<float> planeVertices{
        // positions
        5.0f, -0.5f, 5.0f, -5.0f, -0.5f, -5.0f, -5.0f, -0.5f, 5.0f,

        5.0f, -0.5f, 5.0f, 5.0f,  -0.5f, -5.0f, -5.0f, -0.5f, -5.0f};

    std::vector<float> planeTexCoords{2.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f,

                                      2.0f, 0.0f, 2.0f, 2.0f, 0.0f, 2.0f};

    std::vector<float> transparentVeritces{
        0.0f, 0.5f, 0.0f, 1.0f, -0.5f, 0.0f, 0.0f, -0.5f, 0.0f,

        0.0f, 0.5f, 0.0f, 1.0f, 0.5f,  0.0f, 1.0f, -0.5f, 0.0f};

    std::vector<float> transparentTexCoords{0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

                                            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    std::vector<float> quadVertices{
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f,  -1.0f, 0.0f};

    std::vector<float> quadTexCoords{0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

                                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};

    utility::RawModel cubeModel{cubeVertices, cubeTexCoords, cubeNormals};
    utility::RawModel planeModel{planeVertices, planeTexCoords};
    utility::RawModel squareModel{transparentVeritces, transparentTexCoords};
    utility::RawModel quadModel{quadVertices, quadTexCoords};

    std::vector<glm::vec3> transparentPositions;
    transparentPositions.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    transparentPositions.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    transparentPositions.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    transparentPositions.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    transparentPositions.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    // Load Textures
    [[maybe_unused]] unsigned int cubeTexture =
        utility::loadTexture("res/textures/container.jpg");
    [[maybe_unused]] unsigned int metalTexture =
        utility::loadTexture("res/textures/metal.png");
    [[maybe_unused]] unsigned int grassTexture =
        utility::loadTexture("res/textures/grass.png");
    [[maybe_unused]] unsigned int windowTexture =
        utility::loadTexture("res/textures/blending_transparent_window.png");

    // shader configuration
    // --------------------
    defaultShader.use();
    defaultShader.setInt("texture1", 0);
    defaultShader.setBool("showDepth", false);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    /*
     * Framebuffers
     * Framebuffers have standard opengl object creation
     */

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);  // 1. Generate Frambuffer

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);  // 2. Bind Framebuffer

    /*
     * By binding the framebuffer, all of the following read and write
     * framebuffer operations will only affect the currently bound framebuffer.
     * It is also possible to find a framebuffer to a read or write target
     * specifically
     * by binding to GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER
     * The framebuffer bound to GL_READ_FRAMEBUFFER will be used for all read
     * operations
     * The framebuffer bound to GL_DRAW_FRAMEBUFFER will be user for
     * all write operations
     */

    /*
        The framebuffer is not yet usable as it is not "complete".
        For a framebuffer to be complete it must:
            -   Have at least one buffer (colour, depth, stencil) attached
            -   At least colour attachment
            -   All attachments should be complete
            -   Each buffer should have the same number of samples

        To check if a framebuffer is complete, use glCheckFramebufferStatus()
    */

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Incomplete Framebuffer\n";
    }

    /*
        Remember to unbind and delete framebuffers after use
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    */

    /*
        An attachment is a memory location that can act as a buffer for the
       framebuffer
       When creating an attachment there are two options, textures or
       renderbuffer objects
    */

    // Texture attachments

    unsigned int textureAttachment;
    glGenTextures(1, &textureAttachment);
    glBindTexture(GL_TEXTURE_2D, textureAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.state.screenWidth,
                 window.state.screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*
        The main difference between a texture attachment and a regular texture
        is that we set the texture's data to a nullptr, with this texture we're
        only allocating memory and not actually filling it

        Filling the texture will happen as soon as we render to the framebuffer.

        Now that the texture has been created, it just needs to be attached to
       the active framebuffer
    */

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           textureAttachment, 0);

    /*
        Next to the color attachment, it is also possible to attach depth and
        stencil attachments
    */

    unsigned int depth_stencil_textureAttachment;
    glGenTextures(1, &depth_stencil_textureAttachment);
    glBindTexture(GL_TEXTURE_2D, depth_stencil_textureAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                 window.state.screenWidth, window.state.screenHeight, 0,
                 GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D, depth_stencil_textureAttachment, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Complete Framebuffer\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
        Renderbuffer object attachments
    */

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          window.state.screenWidth, window.state.screenHeight);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Complete Framebuffer\n";
    } else {
        std::cout << "Framebuffer incomplete, ERROR!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
        Cube Maps
    */
    std::vector<std::string> faces{"right.jpg",  "left.jpg",  "top.jpg",
                                   "bottom.jpg", "front.jpg", "back.jpg"};

    stbi_set_flip_vertically_on_load(false);

    [[maybe_unused]] unsigned int cubemapTexture =
        utility::loadCubemap("res/textures/skybox/", faces);

    // Modifying Buffer data
    /*
        glBufferSubData can be used to update the contents of a buffer,
        a call to glBufferData is necessary before calling glBufferSubdata.
        glBufferSubData requires an offset parameter which specifies where
        we want to fill the buffer from

        glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data);
        // Range: [24, 24 + sizeof(data)]
    */

    unsigned int unusedVAO;
    glGenVertexArrays(1, &unusedVAO);
    glBindVertexArray(unusedVAO);

    float data[] = {0.5f, 1.0f, -0.35f};

    unsigned int unusedVBO;
    glGenBuffers(1, &unusedVBO);
    glBindBuffer(GL_VERTEX_ARRAY, unusedVBO);
    glBufferData(GL_VERTEX_ARRAY, 3 * sizeof(float), &data[0], GL_STATIC_DRAW);

    /*
        Another way of getting data into a buffer is to ask for a pointer to the
        buffers memory using glMapBuffer
    */

    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, data, sizeof(data));

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &unusedVAO);
    glDeleteBuffers(1, &unusedVBO);

    /*
        By telling OpenGL we're finished with the pointer operations via
        glUnmapBuffer, OpenGL knows you're done. By unmapping, the pointer
        becomes invalid and the function returns GL_TRUE if OpenGL was able to
        map your data successfully to the buffer.

        Using glMapBuffer is useful for directly mapping data to a buffer,
        without first storing it in temporary memory. Think of directly reading
        data from file and copying it into the buffer's memory.
    */

    // Uniform buffer objects

    unsigned int uniformBlockIndexDefault =
        glGetUniformBlockIndex(defaultShader.ID, "matrices");
    unsigned int uniformBlockIndexEnviro =
        glGetUniformBlockIndex(enivronmentMappingShader.ID, "matrices");

    glUniformBlockBinding(defaultShader.ID, uniformBlockIndexDefault, 0);
    glUniformBlockBinding(enivronmentMappingShader.ID, uniformBlockIndexEnviro,
                          0);

    unsigned int uboMats;
    glGenBuffers(1, &uboMats);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMats);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
                 GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMats, 0, 2 * sizeof(glm::mat4));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
     *      GL_LESS         -   Passes if frag depth is less than stored
     * depth GL_EQUAL        -   Passes if frag depth is equal to stored
     * depth GL_LEQUAL       -   Passes if frag depth is less than or equal
     * to stored depth GL_GREATER      -   Passes if frag depth is greater
     * than stored depth GL_NOTEQUAL     -   Passes if frag depth is not
     * euqal to stored depth GL_GEQUAL       -   Passes if the frag depth is
     * greater than or equal to the stored depth
     */
    glDepthFunc(GL_LESS);  // default values

    /*
     *  Stencil testing
     */
    glEnable(GL_STENCIL_TEST);

    /*
     *  An equivalend to the glDepthMask(GL_FALSE), for the stencil buffer
     * is: glStencilMask(0x00), this ANDs the stencil values with the
     * bitmask 0x00, then all stencil values written to the buffer end up as
     * 0s The default behaviour is the same as setting the bitmask as "0xFF"
     *  glStencilMask(0xFF)
     */
    glStencilMask(0x00);  // each bit ends up as 0, essentially disables
                          // writing to the stencil buffer
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
     *          fragment passes or is discarded. This test function is
     * applied to the stored stencil value and the glStencilFunc's ref
     * value.
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
     *      -   specifies the reference value for the stencil test, the
     * stencil buffer's content is compared against this value.
     *  -   mask
     *      -   Specifies a mask that is ANDed with both the reference value
     * and the stored stencil value before the test compares them
     *      -   Initially set to all 1s
     *
     *  glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
     *  -   sfail
     *      -   action to take if the stencil test fails
     *  -   dpfail
     *      -   action to take if the stencil test passes, but the depth
     * test fails.
     *  -   dppass
     *      -   action to take if both stencil and depth test pass.
     *
     *  Parameters for each of the above parameters are:
     *  -   GL_KEEP
     *      -   the currently stored stencil value is kept
     *  -   GL_ZERO
     *      -   the stencil value is set to 0
     *  -   GL_REPLACE
     *      -   the stencil value is replaced with the reference value set
     * with glStencilFunc
     *  -   GL_INCR
     *      -   Stencil value incremented if lower than maximum value
     *  -   GL_INCR_WRAP
     *      -   Same as GL_INCR, but wraps to 0 if maximum is exceeded
     *  -   GL_DECR
     *      -   Stencil value decremented if higher than minimum value
     *  -   GL_DECR_WRAP
     *      -   Same as GL_DECR, but wraps to the maximum value if lower
     * than minimum
     *  -   GL_INVERT
     *      -   bitwise inverts the current stencil buffer value
     *
     *  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);  // default values
     */

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    /*
     * Blending provides greater flexibility than just discarding unwanted
     * fragments, for blending it needs to be enabled in opengl
     */
    glEnable(GL_BLEND);

    /*
     * To tell opengl how to blend the glBlendFunc function is used.
     * glBlendFunc(GLenum sfactor, GLenum dfactor)
     *  -   sfactor
     *      - The contribution of the source to the final colour
     *  -   dfactor
     *      - The contribution of the desitination to the final colour
     */

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
     * Face culling
     * to enable, simply use glEnable(GL_CULL_FACE)
     */

    glEnable(GL_CULL_FACE);

    /*
     * Set the winding direction using glFrontFace
     * With either GL_CCW for counter-clockwise
     * or GL_CW for clockwise
     *
     */

    glFrontFace(GL_CCW);

    /*
     * Also, tell opengl which face to cull using glCullFace
     * With options as:
     * GL_BACK
     * GL_FRONT
     * GL_FRONT_AND_BACK
     */

    glCullFace(GL_BACK);

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

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = window.state.camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(window.state.camera.Zoom),
                             static_cast<float>(window.state.screenWidth) /
                                 static_cast<float>(window.state.screenHeight),
                             0.1f, 100.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, uboMats);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                        glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                        glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        defaultShader.use();
        defaultShader.setMat4(
            "model", glm::translate(model, glm::vec3(0.0f, -0.001f, 0.0f)));
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        planeModel.draw(defaultShader);

        defaultShader.setMat4(
            "model", glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)));

        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        cubeModel.draw(defaultShader);

        enivronmentMappingShader.use();
        enivronmentMappingShader.setVec3("cameraPos",
                                         window.state.camera.Position);

        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        enivronmentMappingShader.setMat4(
            "model", glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f)));
        enivronmentMappingShader.setBool("shouldReflect", true);
        cubeModel.draw(enivronmentMappingShader);

        enivronmentMappingShader.setMat4(
            "model", glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f)));
        enivronmentMappingShader.setBool("shouldReflect", false);
        cubeModel.draw(enivronmentMappingShader);

        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        cubeModel.draw(skyboxShader);
        glDepthFunc(GL_LESS);

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