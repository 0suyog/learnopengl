#pragma once
#include "camera.h"
#include "model.h"
#include "scene.h"
#include "shader.h"
#include "InputState.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "interval.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_float4x4.hpp"
#include <glm/gtx/string_cast.hpp>
#include "glm/trigonometric.hpp"
#include <iostream>
#include <memory>
#include <vector>
class MoveObjectsWithMouse : public Scene {
public:
  MoveObjectsWithMouse() {}
  std::vector<std::shared_ptr<Model>> models;

  Shader s = Shader("../shaders/shader.vert", "../shaders/shader.frag");
  Shader borderShader =
      Shader("../shaders/shader.vert", "../shaders/bordershader.frag");
  Camera c;
  glm::mat4 view = glm::mat4(1);
  glm::mat4 projection = glm::mat4(1);
  bool drawBorder = false;
  Ray debugRay = Ray(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

  void init() override {
    models.push_back(
        std::make_shared<Model>("../models/woodCube/woodCube.obj"));
    c.position = glm::vec3(0, 0, 5);
    c.yaw = -90;
    c.pitch = 0;
  }

  void update(float deltaTime) override {
    view = c.lookAtMatrix();
    projection = c.perspective_projection();
    c.move(globalWindowState.window, globalInputState.deltaTime);
  }

  // void onMouseDrag(float xPos, float yPos, float dx, float dy) override {
  //   // if (!drawBorder) {
  //   //   c.rotate(dx, dy);
  //   // }
  //
  //   auto rayOrigin = c.position;
  //   auto worldSpace = c.mouseCoordToWorldSpaceAtNearPlane(xPos, yPos);
  //   auto rayDir =
  //       glm::normalize(glm::vec3(xPos, yPos, c.farPlane) -
  //                      glm::vec3(globalWindowState.width,
  //                                globalWindowState.height, c.nearPlane));
  //
  //   for (int i = 0; i < 3; i++) {
  //     std::cerr << worldSpace[i] << "\n";
  //   }
  //   std::cerr << "worldSpace" << "\n";
  //   for (int i = 0; i < 3; i++) {
  //     std::cerr << c.position[i] << "\n";
  //   }
  //   std::cerr << "cam position" << "\n";
  //
  //   Ray r(rayOrigin, rayDir);
  //   r.Draw(s);
  //   if (models[0]->hitBbox(r, infinity)) {
  //     drawBorder = true;
  //   } else {
  //     drawBorder = false;
  //   }
  //   // drawBorder = true;
  // }

  void onMouseClick(int button, int action, int mods) override {
    auto xPos = globalInputState.mousex;
    auto yPos = globalInputState.mousey;
    auto rayOrigin = c.position;
    auto worldSpace = c.mouseCoordToWorldSpaceAtNearPlane(xPos, yPos);
    auto rayDir = glm::normalize(c.direction);

    for (int i = 0; i < 3; i++) {
      std::cerr << worldSpace[i] << "\n";
    }
    std::cerr << "worldSpace" << "\n";
    for (int i = 0; i < 3; i++) {
      std::cerr << c.position[i] << "\n";
    }
    std::cerr << "cam position" << "\n";

    Ray r(rayOrigin, rayDir);
    r.Draw(s);
    if (models[0]->hitBbox(r, infinity)) {
      drawBorder = true;
    } else {
      drawBorder = false;
    }
  }

  void draw() override {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s.use();
    s.setMat4f("view", view);
    s.setMat4f("projection", projection);
    // s.setMat4f("view", glm::mat4(1));
    // s.setMat4f("projection", glm::mat4(1));
    for (auto model : models) {
      s.setMat4f("model",
                 glm::translate(model->modelMatrix(), glm::vec3(0, 0, -10)));
      model->Draw(s);
    }
    if (drawBorder) {
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      glDisable(GL_DEPTH_TEST);
      borderShader.use();
      borderShader.setMat4f("view", view);
      borderShader.setMat4f("projection", projection);
      for (auto model : models) {
        glm::mat4 scaledModel =
            glm::scale(model->modelMatrix(), glm::vec3(1.1, 1.1, 1.1));
        borderShader.setMat4f("model", scaledModel);
        model->Draw(borderShader);
      }
    }
  }
};
