#pragma once
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "shader.h"
class Ray {
public:
  glm::vec3 origin;
  glm::vec3 direction;

  Ray(const glm::vec3 origin, const glm::vec3 direction)
      : origin(origin), direction(direction) {}

  static Ray transform(const Ray &r, const glm::mat4 &transformation);
  void Draw(Shader &shader) {
    glm::vec3 dir = glm::normalize(direction);

    glm::vec3 p0 = origin;
    glm::vec3 p1 = origin + dir * 100.0f; // length

    float vertices[] = {// pos                // normal       // texcoord
                        p0.x, p0.y, p0.z, 0, 0, 1, 0, 0,
                        p1.x, p1.y, p1.z, 0, 0, 1, 0, 0};

    static unsigned int VAO = 0, VBO = 0;

    if (VAO == 0) {
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                   GL_DYNAMIC_DRAW);

      // position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(0);

      // normal (dummy)
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void *)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);

      // texcoord (dummy)
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void *)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
    }

    // update buffer each draw (ray changes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    shader.use();

    // identity model (since you're already in world space)
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4f("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
  }
};

inline Ray Ray::transform(const Ray &r, const glm::mat4 &transformation) {
  glm::vec3 origin = transformation * glm::vec4(r.origin, 1.0);
  glm::vec3 direction =
      glm::normalize(transformation * glm::vec4(r.direction, 0.0));
  return Ray(origin, direction);
}
