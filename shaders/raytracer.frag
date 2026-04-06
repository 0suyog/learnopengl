#version 330 core
in vec3 FragPosition;
out vec4 FragColor;
uniform float time;

struct Material{
  vec3 color; 
};

struct Sphere {
  vec3 origin;
  float radius;
  Material mat;
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

// float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   
//
// float gold_noise(in vec2 xy, in float seed){
//        return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
// }

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
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

vec3 rayColor(Ray r, Sphere[3]world, int maxDepth){
  vec3 color=vec3(1.0,1.0,1.0);
	 for (int i=0;i<maxDepth+1;i++){
	 if (i==maxDepth){
		return vec3(0.0,0.0,0.0);
	 }
	 else{
  HitInfo h;
		HitInfo closestHit;
		Sphere closestSphere;
  float closest=1.0/0.0;
  bool hitAnything=false;
  for (int j=0;j<3;j++){

  if (hitSphere( world[j], r,h) && h.t<closest) {
			 closestHit=h;
			 closest=h.t;
				hitAnything=true;
			 closestSphere=world[j];
		  }
	 }
		if (!hitAnything){
  return color*vec3(0.2,0.15,( r.direction.y+1 )*0.5);
		}
			 r.origin = rayAt(r,closestHit.t);
r.direction = normalize(closestHit.normal + vec3(
    rand(r.origin.xy + float(i) * 0.1),
    rand(r.origin.xy + float(i) * 0.1 + 1.7),
    rand(r.origin.xy + float(i) * 0.1 + 3.3)
) * 2.0 - 1.0);
		color=color*closestSphere.mat.color;
		}
  }
  return color;
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
  Sphere s[3];
  Material red;
  red.color=vec3(1.0,0.0,0.0);
  s[0].origin = vec3(-4.0, 0.0, -5.0);
  s[0].radius = 2.0;
  s[0].mat = red;
  s[1].origin = vec3(-1.0, 0.0, -5.0);
  s[1].radius = 2.0;
  s[1].mat = red;
  s[2].origin = vec3(3.0, 5.0, -5.0);
  s[2].radius = 2.0;
  s[2].mat = red;
  vec3 coord = FragPosition;
  coord.x *= (800.0 / 600.0);
  coord.z = -focal_length;
  vec3 rayDirection = normalize(coord - cameraPosition);
  Ray r = createRay(cameraPosition, rayDirection);
  vec3 color = rayColor(r,s,10);
  // if (hitSphere(s, r,h)) {
  // FragColor = vec4(h.normal, 1.0f);
  // return;
  // }
  // FragColor = vec4(0.0,0.0,( r.direction.y+1 )*0.5,1.0);
FragColor = vec4(
	 color,
    1.0
);
}
