#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
struct Material{
  sampler2D texture_diffuse[8];
  sampler2D texture_specular[8];
};

uniform Material material;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth){
  float ndc = depth*2.0 -1.0;
  return (2.0 * near * far) / (far + near - ndc * (far - near));
}
void main(){
  FragColor = texture(material.texture_diffuse[0], TexCoord);
//   float depthValue = gl_FragCoord.z;
// float linearDepth = LinearizeDepth(depthValue)/far;
//   FragColor = vec4(vec3(linearDepth),1.0f);
}
