#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "InputState.h"
#include "scene.h"
#include "shader.h"
#include "rayTracingCamera.h"
class RayTracedScene : public Scene {

public:
  void init() override {
    glGenVertexArrays(1, &raytracerVAO);
    glGenBuffers(1, &raytracerVBO);
    glBindVertexArray(raytracerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, raytracerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVertices), canvasVertices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glGenBuffers(1, &raytracerEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, raytracerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(canvasIndices), canvasIndices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glGenFramebuffers(1, &raytracerFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, raytracerFBO);

    glGenTextures(1, &rayTracerTexture);
    glBindTexture(GL_TEXTURE_2D, rayTracerTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, globalWindowState.width,
                 globalWindowState.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           rayTracerTexture, 0);

    // renderbufferobject for depth and stencil testing
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

    raytracerShader.use();
    raytracerShader.setInt("prevTexture", 0);

    displayShader.use();
    displayShader.setInt("rayTracedScene", 0);
    glBindTexture(GL_TEXTURE_2D, rayTracerTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cam.initCamera();
    cam.position = glm::vec3(555.0f / 2, 555.0f / 2, 555.0f);
    cam.yaw = 90.0f;
    cam.pitch = 0.0f;
    cam.vfov = 60;
    cam.camSensitivity = 0.1f;
  }
  void update(float deltaTime) override {
    if (glfwGetKey(globalWindowState.window, GLFW_KEY_LEFT_CONTROL) ==
        GLFW_PRESS) {
      cam.moveFast = true;
    }
    raytracerShader.use();
    cam.move(globalWindowState.window, deltaTime);
    cam.update();
    float time = glfwGetTime();
    raytracerShader.setFloat("time", time);
    raytracerShader.setInt("uSamplesPerPixel", samplesPerPixel);
  }

  void onWindowResize(GLFWwindow *window, int width, int height) override {
    cam.handleWindowSizeChange(width, height);
  }

  void onMouseMove(float xPos, float yPos, float dx, float dy) override {
    cam.rotate(dx, dy);
  }

  void draw() override {
    glBindFramebuffer(GL_FRAMEBUFFER, raytracerFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    raytracerShader.use();
    glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    displayShader.use();
    glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

private:
  float canvasVertices[12] = {
      -1.0f, 1.0f,  0.0f, // top left
      -1.0f, -1.0f, 0.0f, // bottom left
      1.0f,  -1.0f, 0.0f, // bottom right
      1.0f,  1.0f,  0.0f, // top right
  };

  unsigned int canvasIndices[6] = {0, 1, 2, 3, 0, 2};

  unsigned int raytracerVAO, raytracerEBO, raytracerVBO, raytracerFBO,
      rayTracerTexture, raytracerRBO;

  Shader raytracerShader =
      Shader("../shaders/raytracer.vert", "../shaders/raytracer.frag");

  Shader displayShader = Shader("../shaders/rayTracerDisplay.vert",
                                "../shaders/rayTracerDisplay.frag");

  RayTracingCamera cam = RayTracingCamera(
      raytracerShader, globalWindowState.width, globalWindowState.height);
  int samplesPerPixel = 8;
};
