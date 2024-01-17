#version 450 core

layout(location = 0) out vec4 FragColor;

#include "uniforms.h"

void main() {
  FragColor = vec4(1, 1, 1, 1);
  FragColor = base_color;
}