#pragma once
#include "glm/common.hpp"
#include "interval.h"
#include "ray.h"
class boundingBox {
public:
  interval xInterval, yInterval, zInterval;
  boundingBox() = default;
  boundingBox(float maxX, float maxY, float maxZ, float minX, float minY,
              float minZ) {
    xInterval = interval(minX, maxX);
    yInterval = interval(minY, maxY);
    zInterval = interval(minZ, maxZ);
  }

  bool hit(Ray r, float closest) const {
    float xo = r.origin.x;
    float yo = r.origin.y;
    float zo = r.origin.z;
    float xd = r.direction.x;
    float yd = r.direction.y;
    float zd = r.direction.z;
    float tminx = (xInterval.min - xo) / xd;
    float tmaxx = (xInterval.max - xo) / xd;
    float tminy = (yInterval.min - yo) / yd;
    float tmaxy = (yInterval.max - yo) / yd;
    float tminz = (zInterval.min - zo) / zd;
    float tmaxz = (zInterval.max - zo) / zd;

    float tmin = fmax(tminz, fmax(tminx, tminy));
    float tmax = fmin(tmaxz, fmin(tmaxx, tmaxy));
    return tmin < tmax;
  }

  int splittingAxis() const {
    if (xInterval.size() > yInterval.size() &&
        xInterval.size() > zInterval.size()) {
      return 0;
    } else if (yInterval.size() > xInterval.size() &&
               yInterval.size() > zInterval.size()) {
      return 1;
    }
    return 2;
  }

  void getAxisValue(int axis, float &min, float &mid, float &max) const {
    if (axis == 1) {
      min = xInterval.min;
      max = xInterval.max;
      mid = (xInterval.max + xInterval.min) / 2;
    }
    if (axis == 2) {
      min = yInterval.min;
      max = yInterval.max;
      mid = (yInterval.max + yInterval.min) / 2;
    }
    if (axis == 3) {
      min = yInterval.min;
      max = yInterval.max;
      mid = (yInterval.max + yInterval.min) / 2;
    }
  }

  glm::vec3 maxValues() const {
    return glm::vec3(xInterval.max, yInterval.max, zInterval.max);
  }

  glm::vec3 minValues() const {
    return glm::vec3(xInterval.min, yInterval.min, zInterval.min);
  }
};
