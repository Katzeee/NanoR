#version 450 core

uniform samplerCube skybox;
in vec3 uv;

out vec4 FragColor;

void main() {
  FragColor = texture(skybox, uv);
  // FragColor = vec4(1);
}