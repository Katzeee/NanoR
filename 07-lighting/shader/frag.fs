#version 330 core

uniform vec3 light_color;
uniform vec3 obj_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

in vec3 normal;
in vec3 frag_pos;
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
  vec3 result = vec3(0.0f);
  float ka = 0.1f;
  float kd = 0.3f;
  float ks = 0.5f;
  vec3 N = normalize(normal);
  vec3 V = normalize(view_pos - frag_pos);
  vec3 L = normalize(light_pos - frag_pos);
  vec3 H = normalize(V + L);
  vec3 R = reflect(-L, N);

  result += ka * light_color;
  result += kd * max(dot(L, N), 0.0) * light_color;
  // result += ks * pow(max(dot(H, N), 0.0), 16.0) * light_color; // may generate incorrect specular
  result += ks * pow(max(dot(R, V), 0.0), 32.0) * light_color;
  FragColor = vec4(result * obj_color, 1.0f);
}
