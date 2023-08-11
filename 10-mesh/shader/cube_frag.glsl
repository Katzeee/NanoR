#version 450 core

// https://wiki.blender.org/wiki/Style_Guide/GLSL

struct PointLight {
  vec3 color;
  vec3 ws_pos;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
};

uniform PointLight light;
uniform vec3 ws_view_pos;
uniform Material mat;

in vec3 P;
in vec3 N;
in vec2 uv;


out vec4 FragColor;

void main() {
  vec3 L = light.ws_pos - P;
  float dis_squared = dot(L, L);
  L = normalize(L);
  vec3 V = normalize(ws_view_pos - P);
  vec3 R = reflect(-L, N);
  vec3 ambient = texture(mat.diffuse, uv).rgb * 0.2;
  vec3 diffuse = max(dot(L, N), 0) * light.color * texture(mat.diffuse, uv).rgb;
  vec3 specular = pow(max(dot(R, V), 0), 4) * light.color * texture(mat.specular, uv).rgb;
  FragColor = vec4(ambient + diffuse + specular, 1);
}
