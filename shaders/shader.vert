#version 330 core
layout (location = 0) in vec3 aPos;
uniform float movingVertex;
out vec4 vertPos;

void main(){
  // gl_Position = vec4(aPos,1.0);
  gl_Position = vec4(aPos.x+movingVertex,aPos.y+movingVertex,aPos.z+movingVertex,1.0);
  vertPos = gl_Position;
};
