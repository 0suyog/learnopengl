#version 330
uniform vec3 movingVertex;
layout (location = 0) in vec3 aPos;
out vec4 vertPos;

void main(){
  gl_Pos = vec4(uniform+aPos,1.0);
  vertPos = gl_Pos;
}
