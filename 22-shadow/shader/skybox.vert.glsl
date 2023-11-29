#version 450 core

layout(location = 0) in vec3 P;

uniform mat4 View;
uniform mat4 Proj;

out vec3 uv;

void main() {
  uv = P;
  vec4 pos = Proj * View * vec4(P, 1);
  gl_Position = pos.xyww;
}