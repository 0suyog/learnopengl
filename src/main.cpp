#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "model.h"
#include <cstdlib>
#include <filesystem>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include "camera.h"

glm::vec3 movementVector;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

double mousex;
double mousey;

double prevMouseX = 0;
double prevMouseY = 0;

void mouseCallback(GLFWwindow *window, double _mouseX, double _mouseY) {
  prevMouseX = mousex;
  prevMouseY = mousey;
  mousex = _mouseX;
  mousey = _mouseY;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenglBySuyog", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouseCallback);

  Shader shaderProgram("../shaders/shader.vert", "../shaders/shader.frag");
  auto model = glm::mat4(1.0f);
  auto view = glm::mat4(1.0f);
  auto projection = glm::mat4(1.0f);
  projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_DEPTH_TEST);

  Model BackPack(std::filesystem::path("../models/backpack/backpack.obj"));

  // camera
  Camera cam;
  float deltaTime = 0;
  float prevFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - prevFrame;
    prevFrame = currentFrame;
    process_input(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model,
        glm::vec3(
            0.0f, 0.0f,
            0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(
        model,
        glm::vec3(4.0f, 4.0f,
                  4.0f)); // it's a bit too big for our scene, so scale down
    shaderProgram.setMat4f("model", model);
    shaderProgram.setMat4f("view", view);
    shaderProgram.setMat4f("projection", projection);
    BackPack.Draw(shaderProgram);
    cam.move(window, deltaTime);
    cam.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    view = cam.lookAtMatrix();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
