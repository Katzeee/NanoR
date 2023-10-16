#version 450 core

in vec2 uv;

uniform vec4 color = vec4(1);
uniform sampler2D tex;

out vec4 FragColor;

void main() { FragColor = color * texture(tex, uv); }
