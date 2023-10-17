#version 450 core

in vec3 P;
in vec3 N;

uniform samplerCube skybox;
uniform vec3 ws_cam_pos;

out vec4 FragColor;

void main() {
  vec3 V = normalize(ws_cam_pos - P);
  float ratio = 1.00 / 1.52;
  vec3 Refract = refract(-V, normalize(N), ratio);
  FragColor = texture(skybox, Refract);
}