#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <system_error>
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
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
  float fov = 45.0f;
  glm::vec3 v = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec3 w = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 u = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  float camSensitivity = 0.1;
  float slowSpeed = 1.5f;
  float fastSpeed = 10.0f;
  bool moveFast = false;
  float focal_length = 1.0;
  Shader shader;

  RayTracingCamera(Shader &shader, int windowWidth, int windowHeight)
      : shader(shader), width(windowWidth), height(windowHeight) {}

  void initCamera() {
    w = glm::vec3(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw)));
    // v = glm::vec3(v, 1.0f);
    // u = lookAtMatrix() * glm::vec4(u, 1.0f);
    float viewPortHeight = 2.0 * tan(glm::radians(fov / 2)) * focal_length;
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
    shader.setInt("width", width);
    shader.setInt("height", height);
    shader.setVec3("delta_u", delta_u);
    shader.setVec3("delta_v", delta_v);
    shader.setVec3("firstPixelLocation", firstPixelLocation);
    shader.setFloat("focal_length", focal_length);
  }

  const glm::mat4 lookAtMatrix() {
    w = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    return glm::lookAt(position, position + w, v);
    shader.use();
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
      movementVector += glm::cross(v, w);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      movementVector -= glm::cross(v, w);
    }
    if (movementVector.x == 0.0 && movementVector.y == 0.0 &&
        movementVector.z == 0) {
      return;
    }
    movementVector = glm::normalize(movementVector);
    auto speed = slowSpeed;
    if (moveFast) {
      speed = fastSpeed;
    }
    position += deltaTime * speed * movementVector;
    shader.use();
    shader.setVec3("camera_position", position);
  }

  void rotate(double prevx, double prevy, float x, float y) {
    if (x == prevx && y == prevy) {
      return;
    }
    yaw += (x - prevx) * camSensitivity;
    pitch -= (y - prevy) * camSensitivity;
    initCamera();
  }

private:
};
