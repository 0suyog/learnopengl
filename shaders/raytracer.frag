#version 330 core
in vec3 FragPosition;
out vec4 FragColor;
uniform float time;

struct Sphere {
  vec3 origin;
  float radius;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct HitInfo {
  vec3 position;
  vec3 normal;
  float t;
};

HitInfo newHitInfo(vec3 position, vec3 normal, float t) {
  HitInfo h;
  h.position = position;
  h.normal = normal;
  h.t = t;
  return h;
}

Ray createRay(vec3 origin, vec3 direction) {
  Ray r;
  r.origin = origin;
  r.direction = direction;
  return r;
}

vec3 rayAt(Ray r, float t) {
  return r.origin + r.direction * t;
}

vec3 sphereNormalAt(Sphere s, vec3 at) {
  return normalize(at - s.origin);
}

float mapToZeroToOne(float x, float max,float min){
  return ( x-min )/(max-min);
}

// LCG random generator(doesnt work as gpu is stateless)
// int seed=88;
// float rand(){
// 	 seed = ( (3434*seed)+7384 )% 100;
// 	 return (mapToZeroToOne(seed,100,0));
// }

// Source - https://stackoverflow.com/a/28095165
// Posted by Birkensox, modified by community. See post 'Timeline' for change history
// Retrieved 2026-04-05, License - CC BY-SA 4.0
//
// Gold Noise ©2015 dcerisano@standard3d.com
// - based on the Golden Ratio
// - uniform normalized distribution
// - fastest static noise generator function (also runs at low precision)
// - use with indicated fractional seeding method. 

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

bool hitSphere(Sphere sphere, Ray r, out HitInfo ht) {
  vec3 direction = normalize(r.direction);
  float a = 1;
  vec3 co = sphere.origin - r.origin;
  float h = dot(co, direction);
  float c = dot(co, co) - (sphere.radius * sphere.radius);
  float discriminant = (h * h) - (a * c);
  if (discriminant < 0) {
    return false;
  }
  float discriminantSqrt = sqrt(discriminant);
  float t = (h - discriminantSqrt) / a;
  // if (discriminant == 0) {
    vec3 p = rayAt(r, t);
    vec3 n = sphereNormalAt(sphere, p);
    ht = newHitInfo(p, n, t);
    return true;
  // }
}

void depthLoop(Ray r, Sphere[3]world, int maxDepth){
	 for (int i=0;i<maxDepth;i++){
	 
  }
}

void main() {
  float aspectRatio = 800.0 / 600.0;
  float viewPortHeight = 2.0;
  float viewPortWidth = aspectRatio * viewPortHeight;
  vec3 viewPort_u = vec3(1.0, 0.0, 0.0);
  vec3 viewPort_v = vec3(0.0, 1.0, 0.0);
  float focal_length = 1.0;
  vec3 viewPort_w = vec3(0.0, 0.0, -focal_length);
  vec3 cameraPosition = vec3(0.0, 0.0, 0.0);
  Sphere s;
  s.origin = vec3(0.0, 0.0, -5.0);
  s.radius = 2.0;
  vec3 coord = FragPosition;
  coord.x *= (800.0 / 600.0);
  coord.z = -focal_length;
  vec3 rayDirection = normalize(coord - cameraPosition);
  Ray r = createRay(cameraPosition, rayDirection);
  HitInfo h;
  // if (hitSphere(s, r,h)) {
  // FragColor = vec4(h.normal, 1.0f);
  // return;
  // }
  // FragColor = vec4(0.0,0.0,( r.direction.y+1 )*0.5,1.0);
FragColor = vec4(
    gold_noise(FragPosition.xy, time),
    gold_noise(FragPosition.xy, time + 1.0),
    gold_noise(FragPosition.xy, time + 2.0),
    1.0
);
}
