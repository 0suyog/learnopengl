#pragma once
#include "camera.h"
#include "glm/ext/matrix_float4x4.hpp"
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
  glm::mat4 model = glm::mat4(1.0f);

public:
  ScreenSpaceToWorldSpace() {};
  void init() override {
    c.width = globalWindowState.width;
    c.height = globalWindowState.height;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    c.yaw = -90.0f;
    c.pitch = 0.0f;
  }
  void onMouseClick(int button, int action, int mods) override {
    if (action == GLFW_PRESS) {
      drawtriangle = true;
      // auto viewSpace = c.mouseCoordToWorldSpace(globalInputState.mousex,
      //                                           globalInputState.mousey);
      // std::cerr << viewSpace.x << " " << viewSpace.y << " " << viewSpace.z
      //           << "\n";
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
    auto projection = c.perspective_projection();
    c.move(globalWindowState.window, deltaTime);
    s.use();
    s.setMat4f("view", view);
    s.setMat4f("projection", projection);
    auto worldSpace = c.mouseCoordToWorldSpaceAtNearPlane(
        globalInputState.mousex, globalInputState.mousey);
    s.setMat4f("model", glm::mat4(1));

    float vertices[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                         0.0f,  0.0f,  0.5f, 0.0f};
    vertices[0] = -0.5f + worldSpace.x;
    vertices[1] = -0.5f + worldSpace.y;
    vertices[3] = 0.5f + worldSpace.x;
    vertices[4] = -0.5f + worldSpace.y;
    vertices[6] = worldSpace.x;
    vertices[7] = worldSpace.y;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // for (int i = 0; i < 9; i++) {
    //   std::cerr << vertices[i] << "\n";
    // }
    // std::cerr << "vertices" << "\n";
    for (int i = 0; i < 3; i++) {
      std::cerr << worldSpace[i] << "\n";
    }
    std::cerr << "worldSpace" << "\n";
    for (int i = 0; i < 3; i++) {
      std::cerr << c.position[i] << "\n";
    }
    std::cerr << "cam position" << "\n";
  }
  void draw() override {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (drawtriangle) {
      s.use();
      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, 3);
    }
  }
};
