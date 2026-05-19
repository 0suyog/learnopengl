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

class bvh {
public:
  std::shared_ptr<node> head;
  std::vector<Triangle> triangles;

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

inline int toArray(const std::shared_ptr<node> n, std::vector<arrayNode> &ans) {
  arrayNode an = arrayNode{.min = n->bbox.minValues(),
                           .isLeafNode = n->isLeafNode,
                           .max = n->bbox.maxValues(),
                           .triStart = n->triStart,
                           .triEnd = n->triEnd};
  ans.push_back(an);
  int ind = ans.size() - 1;
  if (an.isLeafNode) {
    return ind;
  }
  ans[ind].leftInd = toArray(n->left, ans);
  ans[ind].rightInd = toArray(n->right, ans);
  return ind;
}
