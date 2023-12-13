#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 proj;
};

void main() { gl_Position = proj * view * vec4(position, 1); }