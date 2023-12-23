#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 proj;
};

uniform mat4 model;

out VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
vs_out;

void main() {
  gl_Position = proj * view * model * vec4(position, 1);
  vs_out.P = (model * vec4(position, 1)).xyz;
  vs_out.uv = texcoord;
  vs_out.N = mat3(transpose(inverse(model))) * normal;
}