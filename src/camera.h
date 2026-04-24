#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
class Camera {
public:
  float pitch = 45.0f;
  float yaw = -90.0f;
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  float camSensitivity = 0.1;
  float fov = 45.0f;
  float slowSpeed = 1.5f;
  float fastSpeed = 10.0f;
  bool moveFast = false;

  const glm::mat4 lookAtMatrix() {
    direction = -glm::normalize(
        glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
    glm::vec3 vx = glm::cross(up, direction);
    vx = glm::normalize(vx);
    glm::vec3 vy = glm::cross(direction, vx);
    glm::mat4 returnMat(1);
    returnMat[0][0] = vx.x;
    returnMat[1][0] = vx.y;
    returnMat[2][0] = vx.z;
    returnMat[0][1] = vy.x;
    returnMat[1][1] = vy.y;
    returnMat[2][1] = vy.z;
    returnMat[0][2] = direction.x;
    returnMat[1][2] = direction.y;
    returnMat[2][2] = direction.z;
    returnMat[3][0] = -dot(vx, position);
    returnMat[3][1] = -dot(vy, position);
    returnMat[3][2] = -dot(direction, position);
    return returnMat;
    // return glm::lookAt(position, position + direction, vy);
  }

  void move(GLFWwindow *window, float deltaTime) {
    auto movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      movementVector += direction;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      movementVector -= direction;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      movementVector += glm::cross(up, direction);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      movementVector -= glm::cross(up, direction);
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
  }

  void rotate(float dx, float dy) {
    yaw += dx * camSensitivity;
    pitch -= dy * camSensitivity;
  }
};
