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
  void Draw(Shader s) {}
};

inline Ray Ray::transform(const Ray &r, const glm::mat4 &transformation) {
  glm::vec3 origin = transformation * glm::vec4(r.origin, 1.0);
  glm::vec3 direction =
      glm::normalize(transformation * glm::vec4(r.direction, 0.0));
  return Ray(origin, direction);
}
