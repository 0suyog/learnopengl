#version 460
in vec3 pos;
out vec4 FragColor;
uniform sampler2D rayTracedScene;
uniform uint frame;

void main(){
	 vec2 texCoord = ( vec2(pos.x,pos.y) +1  )*0.5;
  // FragColor = vec4(texCoord,0.0,1.0);
	 FragColor =  texture(rayTracedScene,texCoord);
}
