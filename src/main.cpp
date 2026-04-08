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
#include "rayTracingCamera.h"

glm::vec3 movementVector;

int window_width = 800;
int widnow_height = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  window_width = width;
  widnow_height = height;
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

bool firstMouseMovement = true;

void mouseCallback(GLFWwindow *window, double _mouseX, double _mouseY) {
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

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(window_width, widnow_height,
                                        "LearnOpenglBySuyog", NULL, NULL);
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

  Shader raytracerShader("../shaders/raytracer.vert",
                         "../shaders/raytracer.frag");

  float focal_length = 1;
  float vfov = glm::radians(90.0);
  glm::vec3 camera_position = glm::vec3(0.0, 0.0, 0.0);

  RayTracingCamera cam(raytracerShader, window_width, window_width);
  cam.initCamera();
  cam.position = glm::vec3(0.0f, 0.0f, 0.0f);
  cam.yaw = 90.0f;
  cam.pitch = 90.0f;
  glm::mat4 projection =
      glm::perspective(glm::radians(cam.fov),
                       float(window_width) / float(window_width), 0.1f, 100.0f);
  float deltaTime = 0;
  float prevFrame = glfwGetTime();
  int samplesPerPixel = 10;

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - prevFrame;
    prevFrame = currentFrame;
    process_input(window);
    // cam.moveFast = false;
    // cam.move(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      cam.moveFast = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      samplesPerPixel += 1;
      std::cerr << samplesPerPixel << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      samplesPerPixel -= 1;
      std::cerr << samplesPerPixel << std::endl;
    }
    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
    //   std::cerr << "camera Pos: " << cam.position.x << " " << cam.position.y
    //             << " " << cam.position.z << std::endl;
    //   std::cerr << "Yaw: " << cam.yaw << std::endl;
    //   std::cerr << "Pitch: " << cam.pitch << std::endl;
    // }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // cam.move(window, deltaTime);
    // cam.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    // auto view = cam.lookAtMatrix();
    raytracerShader.use();
    cam.rotate(prevMouseX, prevMouseY, mousex, mousey);
    cam.move(window, deltaTime);
    float time = glfwGetTime();
    raytracerShader.setFloat("time", time);
    raytracerShader.setInt("uSamplesPerPixel", samplesPerPixel);
    // raytracerShader.setInt("focal_length", focal_length);
    // raytracerShader.setInt("width", window_width);
    // raytracerShader.setInt("height", widnow_height);
    raytracerShader.setFloat("vfov", vfov);
    glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // commented out to amke a ray tracer shader
    // basicShader.use();
    // basicShader.setMat4f("view", view);
    // basicShader.setMat4f("projection", projection);
    // basicShader.setMat4f("model", backpackModel);
    // backpack.Draw(basicShader);
    // basicShader.setMat4f("model", floorModel);
    // floor.Draw(basicShader);
    // basicShader.setMat4f("model", noiseCubeModel);
    // noiseCube.Draw(basicShader);
    // basicShader.setMat4f("model", woodCubeModel);
    // woodCube.Draw(basicShader);
    prevMouseX = mousex;
    prevMouseY = mousey;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
