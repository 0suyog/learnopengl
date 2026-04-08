#version 330 core
in vec3 FragPosition;
out vec4 FragColor;
uniform float time;
uniform int uSamplesPerPixel;
uniform int width;
uniform int height;
uniform float focal_length;
uniform float vfov;
uniform vec3 camera_position;
uniform vec3 delta_u;
uniform vec3 delta_v;
uniform vec3 firstPixelLocation;

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

float rand( vec2 p )
{
    vec2 K1 = vec2(
        23.14069263277926, // e^pi (Gelfond's constant)
         2.665144142690225 // 2^sqrt(2) (Gelfondâ€“Schneider constant)
    );
    return fract( cos( dot(p,K1) ) * 12345.6789 );
}

vec3 randVec3(vec2 seed){
  return vec3(rand(seed.xy),rand(seed.yx),rand(seed.yx+seed.xy));
}

vec2 randVec2(vec2 seed){
  return vec2(rand(seed.xy),rand(seed.yx));
}

vec3 randVec3InSphere(vec2 seed){
  while(true){
	 vec3 randomVec = ( randVec3(seed)*2.0 )+1.0;
	 if (length(randomVec)<=1){
		if (length(randomVec) < 1e-6) return vec3(1.0, 0.0, 0.0); 
		return randomVec;
	 }
  }
}

vec3 randVec3InHemisphere(vec2 seed, vec3 normal){
  vec3 randomVec = randVec3InSphere(seed);
  if (dot(normal,randomVec)<0){
	 return -randomVec;
  }
  return randomVec;
}

bool hitSphere(Sphere sphere, Ray r, out HitInfo ht, float closestHit) {
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
  if (t<0.001||t>closestHit){
	 if (discriminant == 0) {
		return false;
	 }
	 t=(h+discriminantSqrt)/a;
	 if (t<0.001||t>closestHit){
		return false;
	 }
  }
  vec3 p = rayAt(r, t);
  vec3 n = sphereNormalAt(sphere, p);
  ht = newHitInfo(p, n, t);
  return true;
  // }
}

bool hitSpheres( Sphere[3] world,Ray r,inout HitInfo h){
  HitInfo tempHit;
  float closestSoFar=1.0/0.0;
  bool hitAnything=false;
  for (int i=0;i<3;i++){
	 if (hitSphere(world[i],r,tempHit, closestSoFar)){
		hitAnything=true;
		if (tempHit.t<closestSoFar){
			 closestSoFar = tempHit.t;
		  h=tempHit;
		}
	 }
  }
  return hitAnything;
}

vec3 rayColor(Ray r, Sphere[3]world, int maxDepth){
  vec3 color=vec3(1.0,1.0,1.0);
  for (int i=0;i<maxDepth+1;i++){
	 if (i==maxDepth){
		return vec3(0.0,0.0,0.0);
	 }
	 HitInfo h;
	 if (!hitSpheres(world,r,h)){
		return color*0.5*vec3(1.0,1.0,( r.direction.y+1 )*0.5);
	 }
	 // return ( h.normal+1 )*0.5;
	 // return vec3(1.0,0.3,0.3);
	 color*=0.5*vec3(1.0,0.3,0.3);
	 r.origin = rayAt(r,h.t)+(h.normal*0.01);
	 vec3 randVec = randVec3InHemisphere(r.origin.xy,h.normal);
	 r.direction = normalize(h.normal + randVec);
  }
  return color;
}

vec3 multiSampleLoop(Sphere[3] world,int samplesPerPixel,vec3 origin, vec3 fragCoord){
  vec3 color=vec3(0.0);
  for (int i=0;i<samplesPerPixel;i++){
	 // random point in a -0.5 to 0.5 square
	 vec3 randomSample = vec3(((randVec2(fragCoord.xy+i)*2 )-1 )*0.001,0.0);
	 vec3 rayDir = normalize(( fragCoord-origin )+randomSample);
	 Ray r = createRay(origin,rayDir);
	 color+= rayColor(r,world,2);
  }
  return color/samplesPerPixel;
}

void main() {
  float aspectRatio = float( width ) / float( height );
  float focal_length = 1.0;
  float viewPortHeight = 2.0*tan(vfov/2)*focal_length;
  float viewPortWidth = aspectRatio * viewPortHeight;
  vec3 viewPort_u = vec3(1.0, 0.0, 0.0)*viewPortWidth;
  vec3 viewPort_v = vec3(0.0,-1.0, 0.0)*viewPortHeight;
  // vec3 delta_u = viewPort_u/width;
  // vec3 delta_v = viewPort_v/height;
  vec3 viewPort_w = vec3(0.0, 0.0, -focal_length);
  // vec3 cameraPosition = vec3(0.0, 0.0, 0.0);
  // vec3 firstPixelLocation = camera_position + viewPort_w -(  viewPort_u/2 ) + (viewPort_v/2) + ( delta_u + delta_v )/2;
// FragColor = vec4(width,width,width,1.0);
//   return;
  Sphere s[3];
  Material red;
  red.color=vec3(1.0,0.0,0.0);
  s[0].origin = vec3(0.0, 0.0, -1.0);
  s[0].radius = 0.5;
  s[0].mat = red;
  s[1].origin = vec3(0.0, -100.5, -1.1);
  s[1].radius = 100.0;
  s[1].mat = red;
  s[2].origin = vec3(3.0, 5.0, -5.0);
  s[2].radius = 0.01;
  s[2].mat = red;
  vec3 coord = (FragPosition+1)*0.5;
  coord.x *= width; 
  coord.y*=height;
  coord.z = -focal_length;
  vec3 viewPortPixelCoord = firstPixelLocation + (coord.x*delta_u) - (coord.y*delta_v);
  // vec3 rayDirection = normalize(coord - cameraPosition);
  // Ray r = createRay(cameraPosition, rayDirection);
  vec3 color = multiSampleLoop(s, uSamplesPerPixel,camera_position,viewPortPixelCoord);
  // vec3 color = rayColor(r,s,2);
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
