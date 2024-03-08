#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

#include "uniforms.h"

struct VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
};

layout(location = 0) out VS_OUT vs_out;

void main() {
  gl_Position = proj * view * model * vec4(position, 1.0);
  vs_out.P = (model * vec4(position, 1)).xyz;
  vs_out.uv = texcoord;
  vs_out.N = normalize(mat3(transpose(inverse(model))) * normal);
}