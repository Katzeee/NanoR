#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

uniform vec3 light_color;
// uniform vec3 obj_color;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform Material mat;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coord;
out vec4 FragColor;

vec3 ACESToneMapping(vec3 color, float adapted_lum) {
  const float A = 2.51f;
  const float B = 0.03f;
  const float C = 2.43f;
  const float D = 0.59f;
  const float E = 0.14f;

  color *= adapted_lum;
  return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main() {
  vec3 result = vec3(0.0);
  float ka = 0.4;
  vec3 N = normalize(normal);
  vec3 V = normalize(view_pos - frag_pos);
  vec3 L = normalize(light_pos - frag_pos);
  vec3 H = normalize(V + L);
  vec3 R = reflect(-L, N);

  result += ka * texture(mat.diffuse, tex_coord).xyz * light_color;
  result += texture(mat.diffuse, tex_coord).xyz * max(dot(L, N), 0.0) * light_color;
  result += texture(mat.specular, tex_coord).xyz * pow(max(dot(R, V), 0.0), mat.shininess) * light_color;
  FragColor = vec4(result, 1.0);
}
