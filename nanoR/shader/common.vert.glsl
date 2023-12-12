#version 450
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texcoord;
void main() { 
  gl_Position = vec4(position, 1); 
}