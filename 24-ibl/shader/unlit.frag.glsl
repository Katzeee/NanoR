#version 450 core

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

uniform vec3 color = vec3(1);
uniform sampler2D tex;

out vec4 FragColor;

void main() {
  FragColor = vec4(color * texture(tex, fs_in.uv).rgb, 1.0);
  // FragColor = vec4(color, 1);
}
