#pragma once
#include "model.h"
#include "scene.h"
#include "camera.h"
#include "shader.h"
#include "InputState.h"
#include "glm/ext/matrix_float4x4.hpp"
class RandomBoxScenes : public Scene {
public:
  void init() override {
    // floorModel = glm::scale(floorModel, glm::vec3(2.0f, 2.0f, 2.0f));
    noiseCubeModel =
        glm::translate(noiseCubeModel, glm::vec3(0.0f, 2.01f, 0.0f));
    woodCubeModel =
        glm::translate(woodCubeModel, glm::vec3(1.0f, 2.0001f, 2.0001f));
    backpackModel = glm::translate(backpackModel, glm::vec3(3.0f, 1.85f, 0.0f));
    backpackModel = glm::scale(backpackModel, glm::vec3(0.5f, 0.5f, 0.5f));
  }
  void update(float deltaTime) override {
    if (glfwGetKey(globalWindowState.window, GLFW_KEY_LEFT_SHIFT) ==
        GLFW_PRESS) {
      cam.moveFast = true;
    }
    cam.move(globalWindowState.window, deltaTime);
  }

  void onMouseMove(float xPos, float yPos, float dx, float dy) override {
    cam.rotate(dx, dy);
  }

  void draw() override {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.5, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    basicShader.use();
    basicShader.setMat4f("view", cam.lookAtMatrix());
    basicShader.setMat4f("projection", cam.perspective_projection());
    basicShader.setMat4f("model", backpackModel);
    backpack.Draw(basicShader);
    basicShader.setMat4f("model", floorModel);
    floor.Draw(basicShader);
    basicShader.setMat4f("model", noiseCubeModel);
    noiseCube.Draw(basicShader);
    basicShader.setMat4f("model", woodCubeModel);
    woodCube.Draw(basicShader);
  }

private:
  Camera cam;
  Model floor = Model("../models/floor/floor.obj");
  Model noiseCube = Model("../models/noiseCube/Untitled.obj");
  Model woodCube = Model("../models/woodCube/woodCube.obj");
  Model backpack = Model("../models/backpack/backpack.obj");
  Shader basicShader =
      Shader("../shaders/shader.vert", "../shaders/shader.frag");

  glm::mat4 floorModel = glm::mat4(1.0f);
  glm::mat4 noiseCubeModel = glm::mat4(1.0f);
  glm::mat4 woodCubeModel = glm::mat4(1.0f);
  glm::mat4 backpackModel = glm::mat4(1.0f);
};
