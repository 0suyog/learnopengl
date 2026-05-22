#pragma once
#include "boundingbox.h"
#include "glm/common.hpp"
#include "interval.h"
#include "mesh.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
struct node {
  bool isLeafNode;
  bool isLeft;
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
  // is leaf node and lastis bool isLeft;
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
    float minX = infinity, minY = infinity, minZ = infinity;
    float maxX = -infinity, maxY = -infinity, maxZ = -infinity;
    for (const auto &trig : triangles) {
      minX =
          glm::min(minX, glm::min(trig.p1.x, glm::min(trig.p2.x, trig.p3.x)));
      minY =
          glm::min(minY, glm::min(trig.p1.y, glm::min(trig.p2.y, trig.p3.y)));
      minZ =
          glm::min(minZ, glm::min(trig.p1.z, glm::min(trig.p2.z, trig.p3.z)));
      maxX =
          glm::max(maxX, glm::max(trig.p1.x, glm::max(trig.p2.x, trig.p3.x)));
      maxY =
          glm::max(maxY, glm::max(trig.p1.y, glm::max(trig.p2.y, trig.p3.y)));
      maxZ =
          glm::max(maxZ, glm::max(trig.p1.z, glm::max(trig.p2.z, trig.p3.z)));
    }
    head = split(std::make_shared<node>(
        node{.isLeafNode = false,
             .isLeft = false,
             .bbox = boundingBox(maxX, maxY, maxZ, minX, minY, minZ),
             .triStart = 0,
             .triEnd = int(_triangles.size())}));
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
    int partitionPoint = (parentNode->triStart + parentNode->triEnd) / 2;
    // int leftSide = parentNode->triStart;
    // int rightSide = parentNode->triEnd - 1;
    // while (leftSide <= rightSide) {
    //   if (triangles[leftSide].leftOrRight(axis, min, mid, max) == 1) {
    //     leftSide++;
    //   } else if (triangles[rightSide].leftOrRight(axis, min, mid, max) == 2)
    //   {
    //     rightSide--;
    //   } else {
    //     std::swap(triangles[leftSide], triangles[rightSide]);
    //     leftSide++;
    //     rightSide--;
    //   }
    // }

    // partitionPoint = leftSide;

    if (partitionPoint == parentNode->triStart ||
        partitionPoint == parentNode->triEnd) {
      // std::cerr << partitionPoint << " " << parentNode->triStart << " "
      //           << parentNode->triEnd;
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
             .isLeft = true,
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
             .isLeft = false,
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
  sn.minmax = {
      bboxMin.x,
      bboxMin.y,
      bboxMin.z,
      bboxMax.x,
      bboxMax.y,
      bboxMax.z,
      float(n->isLeafNode),
      float(n->isLeft),
  };
  sn.indices = {n->triStart, n->triEnd, -1, -1};
  shaderNodes.push_back(sn);
  int ind = shaderNodes.size() - 1;
  // std::cerr << ind << " " << n->isLeafNode << "\n";
  if (n->isLeafNode)
    return ind;

  int leftInd = -1;
  int rightInd = -1;

  if (n->left != nullptr)
    leftInd = toArray(n->left, shaderNodes);
  if (n->right != nullptr)
    rightInd = toArray(n->right, shaderNodes);

  // Vector may have reallocated during recursion — index fresh, don't cache
  // pointer/ref
  shaderNodes[ind].indices[2] = leftInd;
  shaderNodes[ind].indices[3] = rightInd;

  return ind;
}
