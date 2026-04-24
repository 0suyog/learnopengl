#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "model.h"
#include "scene.h"
#include <complex>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include "rayTracingCamera.h"
#include "camera.h"
#include "MoveObjectsWithMouseScene.h"
#include "InputState.h"

glm::vec3 movementVector;

Scene *Scene::current = nullptr;
WindowState globalWindowState{nullptr, 800, 600};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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
  globalInputState.prevMousex = globalInputState.mousex;
  globalInputState.prevMousey = globalInputState.mousey;
  globalInputState.mousex = _mouseX;
  globalInputState.mousey = _mouseY;
  globalInputState.deltaX = _mouseX - globalInputState.prevMousex;
  globalInputState.deltaY = _mouseY - globalInputState.prevMousey;
  if (mouseDown) {
    if (Scene::current) {
      Scene::current->onMouseDrag(_mouseX, _mouseY, mousex - _mouseX,
                                  mousey - _mouseY);
    }
  }
  if (firstMouseMovement) {
    firstMouseMovement = false;
    mousex = _mouseX;
    mousey = _mouseY;
  }
  prevMouseX = mousex;
  prevMouseY = mousey;
  mousex = _mouseX;
  mousey = _mouseY;
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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

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
  // Model floor("../models/floor/floor.obj");
  // Model noiseCube("../models/noiseCube/Untitled.obj");
  // Model woodCube("../models/woodCube/woodCube.obj");
  // Model backpack("../models/backpack/backpack.obj");
  // Shader basicShader("../shaders/shader.vert", "../shaders/shader.frag");
  // glm::mat4 floorModel(1.0f);
  // // floorModel = glm::scale(floorModel, glm::vec3(2.0f, 2.0f, 2.0f));
  // glm::mat4 noiseCubeModel(1.0f);
  // noiseCubeModel = glm::translate(noiseCubeModel, glm::vec3(0.0f, 2.01f,
  // 0.0f)); glm::mat4 woodCubeModel(1.0f); woodCubeModel =
  //     glm::translate(woodCubeModel, glm::vec3(1.0f, 2.0001f, 2.0001f));
  // glm::mat4 backpackModel(1.0f);
  // backpackModel = glm::translate(backpackModel, glm::vec3(3.0f, 1.85f,
  // 0.0f)); backpackModel = glm::scale(backpackModel, glm::vec3(0.5f, 0.5f,
  // 0.5f));

  float canvasVertices[] = {
      -1.0f, 1.0f,  0.0f, // top left
      -1.0f, -1.0f, 0.0f, // bottom left
      1.0f,  -1.0f, 0.0f, // bottom right
      1.0f,  1.0f,  0.0f, // top right
  };

  unsigned int canvasIndices[] = {0, 1, 2, 3, 0, 2};

  unsigned int raytracerVAO, raytracerEBO, raytracerVBO;
  glGenVertexArrays(1, &raytracerVAO);
  glGenBuffers(1, &raytracerVBO);
  glBindVertexArray(raytracerVAO);
  glBindBuffer(GL_ARRAY_BUFFER, raytracerVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVertices), canvasVertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glGenBuffers(1, &raytracerEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, raytracerEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(canvasIndices), canvasIndices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);

  // framebuffer for raytracing mainly for temporal accumulation
  unsigned int raytracerFBO;
  glGenFramebuffers(1, &raytracerFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, raytracerFBO);

  // texture attachment for fraebuffer
  unsigned int rayTracerTexture;
  glGenTextures(1, &rayTracerTexture);
  glBindTexture(GL_TEXTURE_2D, rayTracerTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globalWindowState.width,
               globalWindowState.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         rayTracerTexture, 0);

  // renderbufferobject for depth and stencil testing
  unsigned int raytracerRBO;
  glGenRenderbuffers(1, &raytracerRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, raytracerRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        globalWindowState.width, globalWindowState.height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, raytracerRBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "ERROR:FRAMEBUFFER: FRAMEBUFFER ISNOT COMPLETE" << std::endl;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, rayTracerTexture);

  Shader raytracerShader("../shaders/raytracer.vert",
                         "../shaders/raytracer.frag");
  raytracerShader.use();
  raytracerShader.setInt("prevTexture", 0);
  Shader displayShader("../shaders/rayTracerDisplay.vert",
                       "../shaders/rayTracerDisplay.frag");
  displayShader.use();
  displayShader.setInt("rayTracedScene", 0);
  glBindTexture(GL_TEXTURE_2D, rayTracerTexture);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  float focal_length = 1;
  float vfov = glm::radians(90.0);
  glm::vec3 camera_position = glm::vec3(0.0, 0.0, 0.0);

  RayTracingCamera cam(raytracerShader, globalWindowState.width,
                       globalWindowState.height);
  cam.initCamera();
  cam.handleWindowSizeChange(globalWindowState.width, globalWindowState.height);
  cam.position = glm::vec3(0.0f, 0.0f, 0.0f);
  cam.yaw = 90.0f;
  cam.pitch = 90.0f;
  glm::mat4 projection = glm::perspective(glm::radians(cam.fov),
                                          float(globalWindowState.width) /
                                              float(globalWindowState.height),
                                          0.1f, 100.0f);
  float deltaTime = 0;
  float prevFrame = glfwGetTime();
  int samplesPerPixel = 10;
  int frame = 0;

  MoveObjectsWithMouse moveObject;
  moveObject.init();

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    frame++;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - prevFrame;
    globalInputState.deltaTime = deltaTime;
    prevFrame = currentFrame;
    Scene::current = &moveObject;
    moveObject.update(deltaTime);
    moveObject.draw();
    // moveObject.c.move(window, deltaTime);
    // moveObject.c.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    // glBindFramebuffer(GL_FRAMEBUFFER, raytracerFBO);
    process_input(window);
    // // cam.moveFast = false;
    // // cam.move(window, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    //   cam.moveFast = true;
    // }
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    //   samplesPerPixel += 1;
    //   std::cerr << samplesPerPixel << std::endl;
    // }
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //   samplesPerPixel -= 1;
    //   std::cerr << samplesPerPixel << std::endl;
    // }
    // // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
    // //   std::cerr << "camera Pos: " << cam.position.x << " " <<
    // cam.position.y
    // //             << " " << cam.position.z << std::endl;
    // //   std::cerr << "Yaw: " << cam.yaw << std::endl;
    // //   std::cerr << "Pitch: " << cam.pitch << std::endl;
    // // }
    // // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_DEPTH_BUFFER_BIT);
    // // cam.move(window, deltaTime);
    // // cam.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    // // auto view = cam.lookAtMatrix();
    // raytracerShader.use();
    // cam.rotate(prevMouseX, prevMouseY, mousex, mousey);
    // cam.move(window, deltaTime);
    // float time = glfwGetTime();
    // raytracerShader.setFloat("time", time);
    // raytracerShader.setInt("uSamplesPerPixel", samplesPerPixel);
    // // raytracerShader.setInt("focal_length", focal_length);
    // // raytracerShader.setInt("width", window_width);
    // // raytracerShader.setInt("height", widnow_height);
    // raytracerShader.setFloat("vfov", vfov);
    // glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // displayShader.use();
    // // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //
    // // commented out to amke a ray tracer shader
    // // basicShader.use();
    // // basicShader.setMat4f("view", view);
    // // basicShader.setMat4f("projection", projection);
    // // basicShader.setMat4f("model", backpackModel);
    // // backpack.Draw(basicShader);
    // // basicShader.setMat4f("model", floorModel);
    // // floor.Draw(basicShader);
    // // basicShader.setMat4f("model", noiseCubeModel);
    // // noiseCube.Draw(basicShader);
    // // basicShader.setMat4f("model", woodCubeModel);
    // // woodCube.Draw(basicShader);
    prevMouseX = mousex;
    prevMouseY = mousey;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
