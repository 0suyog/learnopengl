#pragma once
#include "scene.h"
#include "GLFW/glfw3.h"
#include "InputState.h"
#include "shader.h"
#include <GL/glext.h>
#include <iostream>
class ScreenSpaceToWorldSpace : public Scene {
  Shader s =
      Shader("../shaders/positiononly.vert", "../shaders/redShader.frag");
  Camera c;
  unsigned int VBO, VAO;
  float vertices[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
  bool drawtriangle = false;

public:
  ScreenSpaceToWorldSpace() {};
  void init() override {
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    c.yaw = -90.0f;
    c.pitch = 0.0f;
  }
  void onMouseClick(int button, int action, int mods) override {
    if (action == GLFW_PRESS) {
      drawtriangle = true;
    }
    if (action == GLFW_RELEASE) {
      drawtriangle = false;
    }
  }

  void onMouseDrag(float xPos, float yPos, float dx, float dy) override {
    // c.rotate(dx, dy);
  }

  void update(float deltaTime) override {
    auto view = c.lookAtMatrix();
    auto projection = glm::perspective(glm::radians(90.0f),
                                       float(globalWindowState.width) /
                                           globalWindowState.height,
                                       0.001f, 1000.0f);
    s.use();
    s.setMat4f("view", view);
    s.setMat4f("projection", projection);
    s.setMat4f("model", glm::mat4(1.0f));
  }
  void draw() override {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (drawtriangle) {
      s.use();
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
  }
};
