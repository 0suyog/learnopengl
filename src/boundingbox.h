#pragma once
#include "interval.h"
#include "ray.h"
#include <iostream>
class boundingBox {
public:
  boundingBox() = default;
  boundingBox(float maxX, float maxY, float maxZ, float minX, float minY,
              float minZ) {
    xInterval = interval(minX, maxX);
    yInterval = interval(minY, maxY);
    zInterval = interval(minZ, maxZ);
  }

  bool hit(Ray r, float closest) {
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

private:
  interval xInterval, yInterval, zInterval;
};
