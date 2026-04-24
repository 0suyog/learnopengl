#pragma once
#include "GLFW/glfw3.h"
struct InputState {
  int mousex;
  int mousey;
  int prevMousex = 0;
  int prevMousey = 0;
  int deltaX;
  int deltaY;
  float deltaTime;
};

inline InputState globalInputState;

struct WindowState {
  GLFWwindow *window;
  int width;
  int height;
};
extern WindowState globalWindowState;
