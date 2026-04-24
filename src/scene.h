#pragma once
#include "camera.h"
#include "GLFW/glfw3.h"
class Scene {
public:
  Scene() {};
  ~Scene() = default;
  virtual void init() = 0;
  virtual void onEnter() {};
  virtual void onExit() {};
  virtual void update(float deltaTime) {};

  virtual void onMouseClick(int button, int action, int mods) {};
  virtual void onMouseDrag(float xPos, float yPos, float dx, float dy) {};
  virtual void onKeyPress(int buttons, int action, int mods) {};
  virtual void onWindowResize(GLFWwindow *window, int width, int height) {};

  virtual void draw() = 0;

  static Scene *current;
};
