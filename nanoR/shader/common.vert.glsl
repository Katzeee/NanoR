#version 450
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texcoord;

uniform mat4 View;

void main() { gl_Position = View * vec4(position, 1); }