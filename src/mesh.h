#pragma once
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "shader.h"
#include <cstddef>
#include <string>
#include <vector>
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Triangle {
  glm::vec3 p1, p2, p3;
  glm::vec3 n;
  glm::vec2 uv1, uv2, uv3;
  bool oneSided;
  float D;
};

struct Texture {
  std::string path;
  unsigned int id;
  std::string type;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : vertices(vertices), indices(indices), textures(textures) {
    setupMesh();
  }

  void Draw(Shader &shader) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for (unsigned int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string number;
      std::string name = textures[i].type;
      if (name == "texture_diffuse") {
        number = std::to_string(diffuseNr++);
      } else if (name == "texture_specular") {
        number = std::to_string(specularNr++);
      }
      shader.setInt("material." + name + "[" + number + "]", i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void loadTrianglesForRayTracing(Shader &shader, glm::mat4 model_matrix) {
    Triangle triangles[vertices.size() / 3];
    for (int i = 0; i < vertices.size(); i += 3) {
      glm::vec3 p1 = model_matrix * glm::vec4(vertices[i].Position, 1.0f);
      glm::vec3 p2 = model_matrix * glm::vec4(vertices[i + 1].Position, 1.0f);
      glm::vec3 p3 = model_matrix * glm::vec4(vertices[i + 2].Position, 1.0f);
      glm::vec3 normal = glm::normalize(glm::cross(p3 - p1, p2 - p1));

      glm::vec2 uv1 = vertices[i].TexCoords;
      glm::vec2 uv2 = vertices[i + 1].TexCoords;
      glm::vec2 uv3 = vertices[i + 2].TexCoords;
      float D = glm::dot(normal, p1);
      triangles[i / 3] =
          Triangle{p1, p2, p3, .n = normal, uv1, uv2, uv3, false, D};
    }
    for (int i = 0; i < vertices.size() / 3; i++) {
      auto triangle = triangles[i];
      auto p1_name = "triangles[" + std::to_string(i) + "].p1";
      auto p2_name = "triangles[" + std::to_string(i) + "].p2";
      auto p3_name = "triangles[" + std::to_string(i) + "].p3";
      auto uv1_name = "triangles[" + std::to_string(i) + "].uv1";
      auto uv2_name = "triangles[" + std::to_string(i) + "].uv2";
      auto uv3_name = "triangles[" + std::to_string(i) + "].uv3";
      auto normal_name = "triangles[" + std::to_string(i) + "].n";
      auto oneSided_name = "triangles[" + std::to_string(i) + "].oneSided";
      auto D_name = "triangles[" + std::to_string(i) + "].D";
      auto matInd_name = "triangles[" + std::to_string(i) + "].matInd";
      shader.setBool(oneSided_name, triangle.oneSided);
      shader.setVec3(p1_name, triangle.p1);
      shader.setVec3(p2_name, triangle.p2);
      shader.setVec3(p3_name, triangle.p3);
      shader.setVec2(uv1_name, triangle.uv1);
      shader.setVec2(uv2_name, triangle.uv2);
      shader.setVec2(uv3_name, triangle.uv3);
      shader.setVec3(normal_name, triangle.n);
      shader.setFloat(D_name, triangle.D);
      shader.setInt(matInd_name, i % 2);
    }
  }

private:
  unsigned int VAO, VBO, EBO;
  void setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
  }
};
