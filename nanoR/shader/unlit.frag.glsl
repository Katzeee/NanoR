#version 450

in vec2 uv;

uniform vec4 color;
uniform sampler2D tex;

out vec4 FragColor;

void main() { FragColor = vec4(color.rgb, texture(tex, uv).a); }