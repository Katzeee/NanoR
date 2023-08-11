#version 450 core

//  which means that the attribute is bound to location 0, so you won't need a call to glGetAttribLocation and can instead do glEnableVertexAttribArray(0);

layout (location = 0) in vec3 lP;
layout (location = 1) in vec3 inN;
layout (location = 2) in vec2 inuv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

out vec3 P;
out vec3 N;
out vec2 uv;


void main() {
  gl_Position = Proj * View * Model * vec4(lP, 1);
  P = (Model * vec4(lP, 1)).xyz;
  N = inN;
  uv = inuv;
}
