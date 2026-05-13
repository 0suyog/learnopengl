#include "RayTracedScene.h"
#include "glm/ext/vector_float3.hpp"
#include "model.h"
#include "scene.h"
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include "InputState.h"

glm::vec3 movementVector;

Scene *Scene::current = nullptr;
WindowState globalWindowState{nullptr, 800, 600};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  if (Scene::current) {
    Scene::current->onWindowResize(window, width, height);
  }
  glViewport(0, 0, width, height);
  globalWindowState.width = width;
  globalWindowState.height = height;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

double mousex;
double mousey;

double prevMouseX;
double prevMouseY;

bool mouseDown = false;

bool firstMouseMovement = true;

void mouseCallback(GLFWwindow *window, double _mouseX, double _mouseY) {
  if (firstMouseMovement) {
    firstMouseMovement = false;
    mousex = _mouseX;
    mousey = _mouseY;
    globalInputState.mousex = _mouseX;
    globalInputState.mousey = _mouseY;
    return;
  }
  prevMouseX = mousex;
  prevMouseY = mousey;
  mousex = _mouseX;
  mousey = _mouseY;
  globalInputState.prevMousex = globalInputState.mousex;
  globalInputState.prevMousey = globalInputState.mousey;
  globalInputState.mousex = _mouseX;
  globalInputState.mousey = _mouseY;
  globalInputState.deltaX = _mouseX - globalInputState.prevMousex;
  globalInputState.deltaY = _mouseY - globalInputState.prevMousey;
  if (mouseDown) {
    if (Scene::current) {
      Scene::current->onMouseDrag(_mouseX, _mouseY, globalInputState.deltaX,
                                  globalInputState.deltaY);
    }
  } else {
    if (Scene::current) {
      Scene::current->onMouseMove(_mouseX, _mouseY, globalInputState.deltaX,
                                  globalInputState.deltaY);
    }
  }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    mouseDown = action == GLFW_PRESS;
  }
  if (Scene::current) {
    Scene::current->onMouseClick(button, action, mods);
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(globalWindowState.width, globalWindowState.height,
                       "LearnOpenglBySuyog", NULL, NULL);
  globalWindowState.window = window;
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
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  stbi_set_flip_vertically_on_load_thread(true);

  float deltaTime = 0;
  float prevFrame = glfwGetTime();
  int frame = 0;

  // MoveObjectsWithMouse moveObject;
  // moveObject.init();
  // ScreenSpaceToWorldSpace screenToWorldSpace;
  // screenToWorldSpace.init();

  RayTracedScene rayTracedScene;
  rayTracedScene.init();

  // RandomBoxScenes randomBoxesScene;
  // randomBoxesScene.init();

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    frame++;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - prevFrame;
    globalInputState.deltaTime = deltaTime;
    prevFrame = currentFrame;

    Scene::current = &rayTracedScene;
    rayTracedScene.update(deltaTime);
    rayTracedScene.draw();

    // Scene::current = &randomBoxesScene;
    // randomBoxesScene.update(deltaTime);
    // randomBoxesScene.draw();

    // Scene::current = &moveObject;
    // moveObject.update(deltaTime);
    // moveObject.draw();

    // Scene::current = &screenToWorldSpace;
    // screenToWorldSpace.update(deltaTime);
    // screenToWorldSpace.draw();

    process_input(window);
    globalInputState.deltaX = 0;
    globalInputState.deltaY = 0;
    globalInputState.prevMousex = globalInputState.mousex;
    globalInputState.prevMousey = globalInputState.mousey;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
