#version 330
layout (location=0) in vec3 Position;
out vec3 pos;

void main(){
  pos = Position;
  gl_Position=vec4(Position,1.0);
}

