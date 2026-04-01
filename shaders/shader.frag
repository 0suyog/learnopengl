#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

struct Material{
  sampler2D texture_diffuse[8];
  sampler2D texture_specular[8];
};

uniform Material material;

void main(){
  FragColor = texture(material.texture_diffuse[0], TexCoord);
}
