#version 450 core

layout(location = 0) in vec3 ls_pos;
layout(location = 2) in vec2 texcoord;

uniform vec3 ws_cam_pos;
uniform mat4 model;
layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 proj;
};

out vec2 uv;

void main() {
  // construct a new local pos which z axis point to cam
  vec3 ls_cam_pos = vec3(inverse(model) * vec4(ws_cam_pos, 1.0));
  vec3 z = normalize(ls_cam_pos);
  vec3 y = vec3(0, 1, 0);
  vec3 x = normalize(cross(y, z));
  y = normalize(cross(z, x));
  // transform from another local pos to real local
  vec3 pos = ls_pos.x * x + ls_pos.y * y + ls_pos.z * z;
  uv = texcoord;
  gl_Position = proj * view * model * vec4(pos, 1.0);
}