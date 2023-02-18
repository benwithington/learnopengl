#include <iostream>
#include <memory>
#include <stdlib.h>
#include <vector>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "shader.h"
#include "camera.h"

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

const int screen_width{800};
const int screen_height{600};

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
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac
#endif

  /* Create a windowed mode window and its OpenGL context */
  std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow) *> window{
      glfwCreateWindow(screen_width, screen_height, "OpenGL", nullptr, nullptr),
      glfwDestroyWindow};

  if (!window) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return EXIT_FAILURE;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window.get());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
  glfwSetKeyCallback(window.get(), key_callback);
  glfwSetCursorPosCallback(window.get(), mouse_callback);
  glfwSetScrollCallback(window.get(), scroll_callback);

  glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Shaders
  Shader default_shader{"shaders/default.vert", "shaders/default.frag"};

  // VAOs and VBOs
  std::vector<float> vertices{
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

  std::vector<unsigned int> indices{
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  std::vector<glm::vec3> cubePositions{
      glm::vec3{0.0f, 0.0f, 0.0f},    glm::vec3{2.0f, 5.0f, -15.0f},
      glm::vec3{-1.5f, -2.2f, -2.5f}, glm::vec3{-3.8f, -2.0f, -12.3f},
      glm::vec3{2.4f, -0.4f, -3.5f},  glm::vec3{-1.7f, 3.0f, -7.5f},
      glm::vec3{1.3f, -2.0f, -2.5f},  glm::vec3{1.5f, 2.0f, -2.5f},
      glm::vec3{1.5f, 0.2f, -1.5f},   glm::vec3{-1.3f, 1.0f, -1.5f}};

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Textures
  unsigned int texture1{};
  unsigned int texture2{};

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_REPEAT); // set texture wrapping to GL_REPEAT (default
                              // wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Textures
  int texture_width{};
  int texture_height{};
  int texture_nr_channels{};
  stbi_set_flip_vertically_on_load(true);

  std::unique_ptr<unsigned char, decltype(stbi_image_free) *> data{
      stbi_load("res/textures/container.jpg", &texture_width, &texture_height,
                &texture_nr_channels, 0),
      stbi_image_free};

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data.get());
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture\n";
  }

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_REPEAT); // set texture wrapping to GL_REPEAT (default
                              // wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  data = {stbi_load("res/textures/awesomeface.png", &texture_width,
                    &texture_height, &texture_nr_channels, 0),
          stbi_image_free};

  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha
    // channel, so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data.get());
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture\n";
  }

  default_shader.use();
  default_shader.setInt("texture1", 0);
  default_shader.setInt("texture2", 1);

  glEnable(GL_DEPTH_TEST);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window.get())) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window.get());

    /* Render here */
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    default_shader.use();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
                                            static_cast<float>(viewport[2]) /
                                                static_cast<float>(viewport[3]),
                                            0.1f, 100.0f);

    default_shader.setMat4("projection", projection);

    glm::mat4 view = camera.getViewMatrix();
    default_shader.setMat4("view", view);

    glBindVertexArray(VAO);
    for (std::size_t i{0}; i < cubePositions.size(); ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      default_shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(window.get());

    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // Function params are required for setting callback but cause unused warnings
  (void)window;

  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {

  // Function params are required for setting callback but cause unused warnings
  (void)scancode;
  (void)mods;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {

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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<float>(yoffset));
}
