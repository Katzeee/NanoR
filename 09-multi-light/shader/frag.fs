#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess; // as specular power
};

struct DirectionLight {
  float intensity;
  vec3 direction;
  vec3 color;
  // you can also divide the color to multiple parts
  // vec3 ambient;
  // vec3 diffuse;
  // vec3 specular;
};

struct PointLight {
  float intensity;
  vec3 position;
  vec3 color;
};

struct SpotLight {
  vec3 position;
  vec3 direction;
  vec3 color;
  float cutoff;
  float outer_cutoff;
};

uniform PointLight p_light;
uniform DirectionLight d_light;
uniform SpotLight s_light;
uniform vec3 view_pos;
uniform Material mat;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coord;
out vec4 FragColor;

void main() {
  float ka = 0.2;
  vec3 N = normalize(normal);
  vec3 V = normalize(view_pos - frag_pos);
  vec3 p_L = normalize(p_light.position - frag_pos); // point light dir
  vec3 p_H = normalize(V + p_L);
  vec3 p_R = reflect(-p_L, N);
  vec3 d_L = normalize(-d_light.direction); // direction light dir
  vec3 d_H = normalize(V + d_L);
  vec3 d_R = reflect(-d_L, N);

  float distance_square =
      dot((p_light.position - frag_pos), (p_light.position - frag_pos));
  float attenuation = 1.0 / distance_square;

  vec3 p_ambient = ka * texture(mat.diffuse, tex_coord).xyz * p_light.color;
  vec3 p_diffuse = max(dot(p_L, N), 0.0) * texture(mat.diffuse, tex_coord).xyz *
                   p_light.color * attenuation * p_light.intensity;
  vec3 p_specular = pow(max(dot(p_R, V), 0.0), mat.shininess) *
                    texture(mat.specular, tex_coord).xyz * p_light.color *
                    attenuation * p_light.intensity;
  vec3 d_diffuse = max(dot(d_L, N), 0.0) * texture(mat.diffuse, tex_coord).xyz *
                   d_light.color * d_light.intensity;
  vec3 d_specular = pow(max(dot(d_R, V), 0.0), mat.shininess) *
                    texture(mat.specular, tex_coord).xyz * d_light.color *
                    d_light.intensity;

  // spot light
  vec3 s_L = normalize(s_light.position - frag_pos);
  vec3 s_H = normalize(V + s_L);
  vec3 s_R = reflect(-s_L, N);
  float theta = dot(s_L, -s_light.direction);
  float epsilon = s_light.cutoff - s_light.outer_cutoff;
  float s_intensity = clamp((theta - s_light.outer_cutoff) / epsilon, 0.0, 1.0);
  vec3 s_diffuse = max(dot(s_L, N), 0.0) * texture(mat.diffuse, tex_coord).xyz *
                   s_light.color * s_intensity;
  vec3 s_specular = pow(max(dot(s_R, V), 0.0), mat.shininess) *
                    texture(mat.specular, tex_coord).xyz * s_light.color *
                    s_intensity;

  vec3 p_acc = p_ambient + p_diffuse + p_specular; // point light accumulation
  vec3 d_acc = d_diffuse + d_specular;
  vec3 s_acc = s_diffuse + s_specular;
  FragColor = vec4(p_acc + d_acc + s_acc, 1.0);
}
