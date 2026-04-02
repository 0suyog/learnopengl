#version 330 core
in vec3 FragPosition;
out vec4 FragColor;

struct Sphere{
  vec3 origin;
  float radius;
};


bool hitSphere(Sphere sphere, vec3 coord){
  if (length(sphere.origin-coord) < sphere.radius){
	 return true;
  }
  return false;
}

void main(){
  Sphere s;
  s.origin = vec3(0.0, 0.0, 0.0);
  s.radius = 1.0;
  vec3 coord = FragPosition;
  coord.x*=(800.0/600.0);
  if (!hitSphere(s,coord)){
  discard;
  }
  FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
}
