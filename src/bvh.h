#pragma once
#include "boundingbox.h"
#include "interval.h"
#include "mesh.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
struct node {
  bool isLeafNode;
  std::shared_ptr<node> left;
  std::shared_ptr<node> right;
  boundingBox bbox;
  int triStart;
  int triEnd;
};

struct arrayNode {
  glm::vec3 min;
  bool isLeafNode;
  glm::vec3 max;
  int leftInd;
  int rightInd;
  int triStart;
  int triEnd;
};

struct ShaderNode {
  // first 6 store minx, miny, minz, maxx and so on and second last is to see if
  // is leaf node and lastis for padding
  std::array<float, 8> minmax;
  // triStart,triEnd, left ind, right ind
  std::array<int, 4> indices;
};

class bvh {
public:
  std::shared_ptr<node> head;
  std::vector<Triangle> triangles;

  bvh() {};

  bvh(std::vector<Triangle> _triangles) {
    triangles = _triangles;
    head = split(std::make_shared<node>(node{
        .isLeafNode = false, .triStart = 0, .triEnd = int(_triangles.size())}));
  };

private:
  std::shared_ptr<node> split(std::shared_ptr<node> parentNode) {
    if (parentNode->triEnd - parentNode->triStart < 10) {
      parentNode->isLeafNode = true;
      return parentNode;
    }
    int axis = parentNode->bbox.splittingAxis();
    float min, max, mid;
    parentNode->bbox.getAxisValue(axis, min, max, mid);
    bool partitioned = false;
    int partitionPoint = parentNode->triStart + parentNode->triEnd / 2;
    int leftSide = parentNode->triStart;
    int rightSide = parentNode->triEnd;

    while (leftSide <= rightSide) {
      if (triangles[leftSide].leftOrRight(axis, min, mid, max) == 1) {
        leftSide++;
      } else if (triangles[leftSide].leftOrRight(axis, min, mid, max) == 2) {
        rightSide--;
      } else {
        std::swap(triangles[leftSide], triangles[rightSide]);
        leftSide++;
        rightSide--;
      }
    }

    partitionPoint = leftSide;

    if (partitionPoint == parentNode->triStart ||
        partitionPoint == parentNode->triEnd) {
      parentNode->isLeafNode = true;
      return parentNode;
    }

    float minX = infinity, minY = infinity, minZ = infinity;
    float maxX = -infinity, maxY = -infinity, maxZ = -infinity;
    for (int i = parentNode->triStart; i < partitionPoint; i++) {
      const auto &t = triangles[i];

      minX = std::min({minX, t.p1.x, t.p2.x, t.p3.x});
      minY = std::min({minY, t.p1.y, t.p2.y, t.p3.y});
      minZ = std::min({minZ, t.p1.z, t.p2.z, t.p3.z});

      maxX = std::max({maxX, t.p1.x, t.p2.x, t.p3.x});
      maxY = std::max({maxY, t.p1.y, t.p2.y, t.p3.y});
      maxZ = std::max({maxZ, t.p1.z, t.p2.z, t.p3.z});
    }

    auto left_node = std::make_shared<node>(
        node{.isLeafNode = false,
             .bbox = boundingBox(maxX, maxY, maxZ, minX, minY, minZ),
             .triStart = parentNode->triStart,
             .triEnd = partitionPoint});
    parentNode->left = split(left_node);

    minX = infinity, minY = infinity, minZ = infinity;
    maxX = -infinity, maxY = -infinity, maxZ = -infinity;

    for (int i = partitionPoint; i < parentNode->triEnd; i++) {
      const auto &t = triangles[i];

      minX = std::min({minX, t.p1.x, t.p2.x, t.p3.x});
      minY = std::min({minY, t.p1.y, t.p2.y, t.p3.y});
      minZ = std::min({minZ, t.p1.z, t.p2.z, t.p3.z});

      maxX = std::max({maxX, t.p1.x, t.p2.x, t.p3.x});
      maxY = std::max({maxY, t.p1.y, t.p2.y, t.p3.y});
      maxZ = std::max({maxZ, t.p1.z, t.p2.z, t.p3.z});
    }
    auto right_node = std::make_shared<node>(
        node{.isLeafNode = false,
             .bbox = boundingBox(maxX, maxY, maxZ, minX, minY, minZ),
             .triStart = partitionPoint,
             .triEnd = parentNode->triEnd});
    parentNode->right = split(right_node);
    return parentNode;
  }
};

inline int toArray(const std::shared_ptr<node> n,
                   std::vector<ShaderNode> &shaderNodes) {
  ShaderNode sn;
  auto bboxMax = n->bbox.maxValues();
  auto bboxMin = n->bbox.minValues();
  sn.minmax = {bboxMin.x,
               bboxMin.y,
               bboxMin.z,
               bboxMax.x,
               bboxMax.y,
               bboxMax.z,
               float(n->isLeafNode),
               0.0};
  sn.indices = {n->triStart, n->triEnd};

  shaderNodes.push_back(sn);
  int ind = shaderNodes.size() - 1;
  if (n->isLeafNode) {
    return ind;
  }
  shaderNodes[ind].indices[2] = toArray(n->left, shaderNodes);
  shaderNodes[ind].indices[3] = toArray(n->right, shaderNodes);
  return ind;
}
