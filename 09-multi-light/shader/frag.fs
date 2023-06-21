#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess; // as specular power
};

struct DirectionLight {
  vec3 direction;
  vec3 color;
  // you can also divide the color to multiple parts
  // vec3 ambient;
  // vec3 diffuse;
  // vec3 specular;
};

struct PointLight {
  vec3 position;
  vec3 color;
};

uniform PointLight p_light;
uniform DirectionLight d_light;
uniform vec3 view_pos;
uniform Material mat;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coord;
out vec4 FragColor;

void main() {
  vec3 result = vec3(0.0);
  float ka = 0.4;
  vec3 N = normalize(normal);
  vec3 V = normalize(view_pos - frag_pos);
  vec3 L = normalize(p_light.position - frag_pos);
  vec3 H = normalize(V + L);
  vec3 R = reflect(-L, N);

  result += ka * texture(mat.diffuse, tex_coord).xyz * p_light.color;
  result +=
      texture(mat.diffuse, tex_coord).xyz * max(dot(L, N), 0.0) * p_light.color;
  result += texture(mat.specular, tex_coord).xyz *
            pow(max(dot(R, V), 0.0), mat.shininess) * p_light.color;
  FragColor = vec4(result, 1.0);
}
