#version 450 core

uniform vec4 color = vec4(1);

out vec4 FragColor;

void main() { FragColor = vec4(color.rgb, 1); }
