#version 450 core

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
} fs_in;

uniform vec4 color = vec4(1);
uniform sampler2D tex;

out vec4 FragColor;

void main() { FragColor = color * texture(tex, fs_in.uv); }
