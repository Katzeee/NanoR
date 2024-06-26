#version 450

struct FS_IN {
  vec3 P;
  vec3 N;
  vec2 uv;
};

layout(location = 0) in FS_IN fs_in;
layout(location = 0) out vec4 FragColor;

void main() { 
  // FragColor = vec4(color.rgb, texture(albedo, fs_in.uv).a); 
  FragColor = vec4(1, 0, 1, 1); 
}