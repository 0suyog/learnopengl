#pragma once
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "shader.h"
class RayTracingCamera {
public:
  float aspectRatio = 1.0;
  float pitch = 45.0f;
  float yaw = 90.0f;
  int width;
  int height;
  float vfov = 45.0f;

  glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 w = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 u = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  float camSensitivity = 0.3f;
  float slowSpeed = 50.0f;
  float fastSpeed = 100.0f;
  bool moveFast = false;
  float focal_length = 1.0;
  Shader shader;

  RayTracingCamera(Shader &shader, int windowWidth, int windowHeight)
      : shader(shader), width(windowWidth), height(windowHeight) {}

  void initCamera() {
    frame = 1;
    aspectRatio = float(width) / float(height);

    w = -glm::normalize(
        glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
    u = glm::cross(w, vup);
    v = glm::cross(w, u);
    float viewPortHeight = 2.0 * tan(glm::radians(vfov / 2)) * focal_length;
    float viewPortWidth = aspectRatio * viewPortHeight;
    glm::vec3 viewPort_u = u * viewPortWidth;
    glm::vec3 viewPort_v = v * viewPortHeight;
    glm::vec3 viewPort_w = w * focal_length;
    glm::vec3 delta_u = viewPort_u / float(width);
    glm::vec3 delta_v = viewPort_v / float(height);
    glm::vec3 firstPixelLocation =
        position + viewPort_w - (viewPort_u / float(2)) +
        (viewPort_v / float(2)) + (delta_u + delta_v) / float(2);

    shader.use();
    shader.setVec3("camera_position", position);
    shader.setInt("width", width);
    shader.setInt("height", height);
    shader.setVec3("delta_u", delta_u);
    shader.setVec3("delta_v", delta_v);
    shader.setVec3("firstPixelLocation", firstPixelLocation);
    shader.setFloat("focal_length", focal_length);
    shader.setUInt("vfov", vfov);
  }

  void move(GLFWwindow *window, float deltaTime) {
    auto movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      movementVector += w;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      movementVector -= w;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      movementVector -= u;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      movementVector += u;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      movementVector += vup;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      movementVector -= vup;
    }
    auto speed = slowSpeed;
    if (moveFast) {
      speed = fastSpeed;
      moveFast = false;
    }
    if (movementVector.x == 0.0 && movementVector.y == 0.0 &&
        movementVector.z == 0) {
      return;
    }
    movementVector = glm::normalize(movementVector);
    position += deltaTime * speed * movementVector;
    initCamera();
  }

  void update() {
    frame++;
    shader.setUInt("frame", frame);
  }

  void rotate(float dx, float dy) {
    if (!dx && !dy) {
      return;
    }
    yaw += dx * camSensitivity;
    pitch -= (dy * camSensitivity);
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    initCamera();
  }

  void handleWindowSizeChange(int window_width, int window_height) {
    if (window_width == width && window_height == height) {
      return;
    }
    width = window_width;
    height = window_height;
    initCamera();
  }

private:
  uint frame = 1;
};
