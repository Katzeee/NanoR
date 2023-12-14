#version 450 core

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

out vec4 FragColor;

void main() {
  FragColor = vec4(0.5, 0.5, 0.5, 1);
}