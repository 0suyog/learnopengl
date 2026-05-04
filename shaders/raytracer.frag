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
uniform sampler2D prevTexture;
uniform uint frame;

const uint LAMBERTIAN = 1u;
const uint METAL = 2u;
const uint NORMAL = 3u;
const uint LIGHT = 4u;
const uint DIELECTRIC = 5u;


struct Material{
  uint type;
  vec3 albedo;
  vec3 emission;
  float fuzz;
};

struct Sphere {
  vec3 origin;
  float radius;
  Material mat;
};

struct Quad {
  vec3 bottomLeft;
  vec3 u;
  vec3 v;
  vec3 n;
  bool oneSided;
  // the D of Ax+By+Cz=D where (A,B,C) is normal and (x,y,z) is the point so it is dot(n,bottomLeft)
  float D;
  Material mat;
};

Quad CreateQuad(vec3 bottomLeft, vec3 u, vec3 v){
  Quad q;
  q.bottomLeft = bottomLeft;
  q.u=u;
  q.v=v;
  q.n=cross(u,v);
  q.D = dot(q.n,bottomLeft);
  return q;
}

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct HitInfo {
  vec3 position;
  vec3 normal;
  float t;
  Material mat;
};

HitInfo newHitInfo(vec3 position, vec3 normal, float t, Material mat) {
  HitInfo h;
  h.position = position;
  h.normal = normal;
  h.t = t;
  h.mat=mat;
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

bool isFrontFace(vec3 incomingDir,vec3 normal){
  return dot(incomingDir,normal)<0;
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
// float rand(in vec2 xy){
//        return fract(tan(distance(xy*PHI, xy)*232.2983)*xy.x);
// }

// float rand( vec2 p )
// {
//     vec2 K1 = vec2(
//         23.14069263277926, // e^pi (Gelfond's constant)
//          2.665144142690225 // 2^sqrt(2) (Gelfondâ€“Schneider constant)
//     );
//     return fract( cos( dot(p,K1) ) * 12345.6789 );
// }
// float rand(vec2 p) {
//     p = fract(p * vec2(123.34, 456.21));
//     p += dot(p, p + 78.233);
//     return fract(p.x * p.y);
// }

vec2 gSeed = vec2(
  float(frame) + FragPosition.x,
  float(frame) + FragPosition.y
);

float rand() {
    gSeed =  vec2(fract(sin(dot(gSeed, vec2(12.9898,78.233))) * 43758.5453),fract(sin(dot(gSeed.yx, vec2(12.9898,78.233))) * 43758.5453));
}

vec3 randVec3(){
  return vec3(rand(gSeed.xy),rand(gSeed.yx),rand(gSeed.yx+gSeed.xy));
}

vec2 randVec2(){
  return vec2(rand(gSeed.xy),rand(gSeed.yx));
}

vec3 randVec3InSphere(){
  int count = 0;
  while(true){
	 vec3 randomVec = (randVec3()-0.5)*2;
	 float len = length(randomVec);
	 if (len<=1){
		if (len < 1e-6) return vec3(1.0, 0.0, 0.0); 
		return randomVec;
	 }
	 if (count > 10){
	  return vec3(1.0, 0.0, 0.0); 
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
  n = isFrontFace(r.direction,n) ? n: -n;
  ht = newHitInfo(p, n, t, sphere.mat);
  return true;
}

bool hitQuad(Quad q, Ray r, inout HitInfo ht, float closestHit){
  float denom = dot(q.n,r.direction);
  if (abs( denom ) <= 1e-8){
		return false;
  }
  float np = dot(q.n,r.origin);
  float t = (q.D-np)/denom;
  if (t<0.001 || t>=closestHit){
	 return false;
  }
  vec3 p = rayAt(r,t);
  vec3 relativeP = p-q.bottomLeft;
  // checking if the hitpoint lies in the quad
  float a = dot(q.u,q.u);
  float b = dot(q.u,q.v);
  float c = dot(relativeP,q.u);
  float d = dot(q.v,q.v);
  float e = dot(relativeP,q.v);
  float beta = (( a*e )-(b*c))/((a*d)-(b*b));
  float alpha = (c-(b*beta))/a;
  if (beta >1 || beta <0 || alpha < 0 || alpha >1){
	 return false;
  }
  bool frontFace=isFrontFace(r.direction,q.n);
  if (q.oneSided){
	 if (!frontFace){
		return false;
	 }
  }
  vec3 n = isFrontFace(r.direction,q.n)?q.n:-q.n;
  ht = newHitInfo(p, n, t, q.mat);
  return true;
}

bool hitSpheres( Sphere[3] world,Ray r,inout HitInfo h, float closest){
  HitInfo tempHit;
  float closestSoFar=closest;
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

bool hitQuads(Quad[7] world,Ray r,inout HitInfo h, float closest){
  HitInfo tempHit;
  float closestSoFar=closest;
  bool hitAnything=false;
  for (int i=0;i<7;i++){
	 if (hitQuad(world[i],r,tempHit, closestSoFar)){
		hitAnything=true;
		if (tempHit.t<closestSoFar){
			 closestSoFar = tempHit.t;
		  h=tempHit;
		}
	 }
  }
  return hitAnything;
}

vec3 lambertianReflection(vec3 normal,vec2 seed){
	 vec3 randVec = randVec3InHemisphere(seed,normal);
  return normalize(randVec);
}


vec3 reflect(vec3 incidentDir,vec3 normal){
 return incidentDir-2.0*dot(incidentDir,normal)*normal;
}

bool scatter(Ray r_in, HitInfo h, out vec3 albedo, out Ray scattered){
  switch(h.mat.type){
	 case (LAMBERTIAN):{
		scattered.origin = h.position;
		scattered.direction = lambertianReflection(h.normal,gSeed+time);
		albedo = h.mat.albedo;
		return true;
	 };
	 case (METAL):{
		scattered.origin = h.position;
		scattered.direction = normalize(reflect(r_in.direction, h.normal)+(randVec3InSphere((h.position+h.normal+time).xy)*h.mat.fuzz*0.5));
		albedo = h.mat.albedo;
		return true;
	 };
	 default:{
		return false;
	 }
  }
}

vec3 emit(Ray r_in,HitInfo h){
  switch(h.mat.type){
	 case(LIGHT):{
		return h.mat.emission;
	 }
	 default:{
		 return vec3(0.0,0.0,0.0);
	 }
  }
}


vec3 rayColor(Ray r, Sphere[3]world,Quad[7] quads, int maxDepth){
  vec3 color = vec3(0.0,0.0,0.0);
  vec3 throughPut=vec3(1.0,1.0,1.0);
  for (int i=0;i<maxDepth+1;i++){
	 gSeed+=rand(vec2(i*53,i*37));
	 if (i==maxDepth){
		return color;
		// return vec3(0.0,0.0,0.0);
	 }
	 bool hitAnything=false;
	 HitInfo h;
	 if (hitSpheres(world,r,h,1.0/0.0)){
	  hitAnything=true; 
	 // return vec3( (r.direction.x+1)*0.5,(r.direction.y+1)*0.5,(r.direction.z+1)*0.5 );
	 }
	 float closest = 1.0/0.0;
	 if(hitAnything){
		closest = h.t;
	 }
    if(hitQuads(quads,r,h,closest)){
	 hitAnything=true;
	 }
	 // return vec3(float(hitAnything));
	 if(!hitAnything){
		// return vec3(0.0,0.0,0.0);
		return vec3(0.6,0.3,0.2);

	 }
	 // return ( h.normal+1 )*0.5;
	 // return vec3(1.0,0.3,0.3);
	 if (h.mat.type==NORMAL){
		return (normalize(h.normal)+1 )*0.5;
	 }
	 vec3 albedo;
	 vec3 emission;
	 Ray scatteredRay;
	 emission =  emit(r,h);
	 color+= throughPut*emission;
	 if (!scatter(r,h,albedo,scatteredRay)){
		return color;
	 }
	 throughPut *= albedo;
	 r=scatteredRay;
  }
  return color;
}

vec3 multiSampleLoop(Sphere[3] world,Quad[7] q,int samplesPerPixel,vec3 origin, vec3 fragCoord){
  vec3 color=vec3(0.0);
  for (int i=0;i<samplesPerPixel;i++){
	 gSeed = fragCoord.xy +rand(vec2(fragCoord.x+ i*37,fragCoord.y+i*67));
	 vec3 randomSample = vec3((randVec2(fragCoord.xy+i)-0.5),0.0);
	 vec3 offset = randomSample.x*delta_u + randomSample.y*delta_v;
	 vec3 rayDir = normalize(( fragCoord-origin )+offset);
	 Ray r = createRay(origin,rayDir);
	 color+= rayColor(r,world,q,10);
  }
  color= color/samplesPerPixel;
  return color;
}

void main() {
  Material white_diffuse;
  white_diffuse.type = LAMBERTIAN;
  white_diffuse.albedo = vec3(0.73, 0.73, 0.73);

  Material red_diffuse;
  red_diffuse.type = LAMBERTIAN;
  red_diffuse.albedo = vec3(0.85, 0.35, 0.35);

  Material green_diffuse;
  green_diffuse.type = LAMBERTIAN;
  green_diffuse.albedo = vec3(0.12, 0.45, 0.15);

  Material blue_diffuse;
  blue_diffuse.type = LAMBERTIAN;
  blue_diffuse.albedo = vec3(0.35, 0.45, 0.85);

  Material yellow_diffuse;
  yellow_diffuse.type = LAMBERTIAN;
  yellow_diffuse.albedo = vec3(0.9, 0.8, 0.4);

  Material silver_metal;
  silver_metal.type = METAL;
  silver_metal.albedo = vec3(0.9, 0.9, 0.9);
  silver_metal.fuzz = 0.0;

  Material gold_metal;
  gold_metal.type = METAL;
  gold_metal.albedo = vec3(0.8, 0.6, 0.2);
  gold_metal.fuzz = 0.05;

  Material copper_metal;
  copper_metal.type = METAL;
  copper_metal.albedo = vec3(0.9, 0.5, 0.3);
  copper_metal.fuzz = 0.1;

  Material rough_metal;
  rough_metal.type = METAL;
  rough_metal.albedo = vec3(0.6, 0.6, 0.6);
  rough_metal.fuzz = 0.25;

  Material blue_metal;
  blue_metal.type = METAL;
  blue_metal.albedo = vec3(0.4, 0.5, 0.9);
  blue_metal.fuzz = 0.08;

  Material white_light;
  white_light.type = LIGHT;
  white_light.emission = vec3(150.0, 150.0, 150.0);

  Material warm_light;
  warm_light.type = LIGHT;
  warm_light.emission = vec3(255.0, 220.0, 120.0);

  Material cool_light;
  cool_light.type = LIGHT;
  cool_light.emission = vec3(120.0, 180.0, 255.0);

  Material red_light;
  red_light.type = LIGHT;
  red_light.emission = vec3(200.0, 80.0, 80.0);

  Material blue_light;
  blue_light.type = LIGHT;
  blue_light.emission = vec3(80.0, 80.0, 200.0);

  Sphere s[3];
  Material normal;
  normal.type=NORMAL;
  Material white;
  white.type=LAMBERTIAN;
  white.albedo=vec3(0.73,0.73,0.73);
  Material red;
  red.type=METAL;
  red.fuzz=0.2;
  Material green;
  green.type=LAMBERTIAN;
  green.fuzz=0.1;
  Material light;
  light.type=LIGHT;
  light.albedo=vec3(0.58, 0.173, 0.259);
  red.albedo=vec3(0.85,0.4,0.4);
  green.albedo = vec3( 0.12,0.45,0.15 );
  light.emission = vec3(150.0,150.0,150.0);
  s[0].origin = vec3(80.0, 40.0, 60.0);
  s[0].radius = 40;
  s[0].mat = red;
  s[1].origin = vec3(200.0, 120.0, 400.0);
  s[1].radius = 100.0;
  s[1].mat = green;
  s[2].origin = vec3(0, 7.0, 0.0);
  s[2].radius = 0.1;
  s[2].mat = light;

  Quad q[7];


  // Left wall (green) — x = 555, normal = -X
  q[0] = CreateQuad(
	 vec3(555, 0, 0),
	 vec3(0, 0, 555),
	 vec3(0, 555, 0)
  );
  q[0].mat = blue_diffuse;
  q[0].oneSided = true;


  // Right wall (red) — x = 0, normal = +X
  q[1] = CreateQuad(
	 vec3(0, 0, 0),
	 vec3(0, 555, 0),
	 vec3(0, 0, 555)
  );
  q[1].mat = rough_metal;
  q[1].oneSided = true;
  // Light (ceiling rectangle) — keep double-sided
  q[2] = CreateQuad(
	 vec3(343, 554, 332),
	 vec3(-130, 0, 0),
	 vec3(0, 0, -105)
  );
  q[2].mat = light;
  // q[2].oneSided = false; // IMPORTANT: leave it off


  // Floor — y = 0, normal = +Y
  q[3] = CreateQuad(
	 vec3(0, 0, 0),
	 vec3(0, 0, 555),
	 vec3(555, 0, 0)
  );
  q[3].mat = blue_diffuse;
  q[3].oneSided = true;


  // Ceiling — y = 555, normal = -Y
  q[4] = CreateQuad(
	 vec3(555, 555, 555),
	 vec3(-555, 0, 0),
	 vec3(0, 0, -555)
  );
  q[4].mat = blue_diffuse;
  q[4].oneSided = true;


  // Back wall — z = 555, normal = -Z
  q[5] = CreateQuad(
	 vec3(0, 0, 555),
	 vec3(0, 555, 0),
	 vec3(555, 0, 0)
  );
  q[5].mat = blue_diffuse;
  q[5].oneSided = true;


  // Front wall — z = 0, normal = +Z
  q[6] = CreateQuad(
	 vec3(0, 0, 0),
	 vec3(555, 0, 0),
	 vec3(0, 555, 0)
  );
  q[6].mat = yellow_diffuse;
  q[6].oneSided = true;

  vec3 coord = (FragPosition+1)*0.5;
  coord.x *= width; 
  coord.y*=height;
  coord.z = -focal_length;
  vec3 viewPortPixelCoord = firstPixelLocation + (coord.x*delta_u) - (coord.y*delta_v);
  vec3 color = multiSampleLoop(s,q,uSamplesPerPixel,camera_position,viewPortPixelCoord);
  vec4 pervColor = texture(prevTexture,((FragPosition+1)*0.5 ).xy);
  FragColor =mix(pervColor, vec4(color, 1.0), 1.0 / float(frame));

  if (FragColor.x>=1.0/0.0||FragColor.y>=1.0/0.0||FragColor.z>=1.0/0.0){
	 FragColor = vec4(0.0, 1, 0.0,1.0);
  }
  // FragColor = vec4(vec3(frame),1.0);
}
