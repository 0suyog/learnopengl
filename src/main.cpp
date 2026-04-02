#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "model.h"
#include <complex>
#include <cstdlib>
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
  glEnable(GL_DEPTH_TEST);
  stbi_set_flip_vertically_on_load_thread(true);
  Model floor("../models/floor/floor.obj");
  Model noiseCube("../models/noiseCube/Untitled.obj");
  Model woodCube("../models/woodCube/woodCube.obj");
  Model backpack("../models/backpack/backpack.obj");
  Shader basicShader("../shaders/shader.vert", "../shaders/shader.frag");
  glm::mat4 floorModel(1.0f);
  // floorModel = glm::scale(floorModel, glm::vec3(2.0f, 2.0f, 2.0f));
  glm::mat4 noiseCubeModel(1.0f);
  noiseCubeModel = glm::translate(noiseCubeModel, glm::vec3(0.0f, 2.0f, 0.0f));
  glm::mat4 woodCubeModel(1.0f);
  woodCubeModel = glm::translate(woodCubeModel, glm::vec3(1.0f, 2.0f, 2.0f));
  glm::mat4 backpackModel(1.0f);
  backpackModel = glm::translate(backpackModel, glm::vec3(3.0f, 1.85f, 0.0f));
  backpackModel = glm::scale(backpackModel, glm::vec3(0.5f, 0.5f, 0.5f));
  Camera cam;
  glm::mat4 projection =
      glm::perspective(glm::radians(cam.fov), 800.0f / 600.0f, 0.1f, 100.0f);
  float deltaTime = 0;
  float prevFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - prevFrame;
    prevFrame = currentFrame;
    process_input(window);
    cam.moveFast = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      cam.moveFast = true;
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam.move(window, deltaTime);
    cam.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    auto view = cam.lookAtMatrix();
    basicShader.use();
    basicShader.setMat4f("view", view);
    basicShader.setMat4f("projection", projection);
    basicShader.setMat4f("model", backpackModel);
    backpack.Draw(basicShader);
    basicShader.setMat4f("model", floorModel);
    floor.Draw(basicShader);
    basicShader.setMat4f("model", noiseCubeModel);
    noiseCube.Draw(basicShader);
    basicShader.setMat4f("model", woodCubeModel);
    woodCube.Draw(basicShader);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
