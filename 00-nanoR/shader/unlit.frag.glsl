#version 450

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

uniform vec4 color;
uniform sampler2D albedo;

out vec4 FragColor;

void main() { FragColor = vec4(color.rgb, texture(albedo, fs_in.uv).a); }