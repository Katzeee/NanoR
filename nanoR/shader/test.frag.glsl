#version 450 core

struct FS_IN {
  vec3 P;
  vec3 N;
  vec2 uv;
};

layout(location = 0) in FS_IN fs_in;

layout(location = 0) out vec4 FragColor;

#include "uniforms.h"

const float PI = 3.14159265359;

// clamp to avoid black points
vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0); }

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a_square = a * a;
  float nh = max(dot(N, H), 0.0);
  float denom = (nh * nh) * (a_square - 1.0) + 1.0;
  denom = PI * denom * denom;
  return a_square / denom;
}

float GeometrySchlickGGX(vec3 N, vec3 D, float roughness) {
  float nd = max(dot(N, D), 0.0);
  float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
  float denom = nd * (1.0 - k) + k;
  return nd / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float G1nl = GeometrySchlickGGX(N, L, roughness);
  float G1nv = GeometrySchlickGGX(N, V, roughness);
  return G1nl * G1nv;
}

void main() {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);
  vec3 N = normalize(fs_in.N);
  vec3 V = normalize(ws_cam_pos - fs_in.P);
  // vec3 R = normalize(reflect(-V, N));
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, base_color.rgb, metallic);

  // SECTION
  PointLight light = p_light;
  vec3 L = light.ws_position - fs_in.P;
  float dis_2 = dot(L, L);
  L = normalize(L);
  vec3 H = normalize(L + V);
  float nl = max(dot(N, L), 0.0);
  vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
  vec3 Ks = F;
  vec3 Kd = vec3(1.0) - Ks;
  Kd *= 1.0 - metallic;
  float D = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);

  vec3 nom = D * G * F;
  // avoid devide 0
  float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
  specular += nom / denom * light.color * light.intensity * nl / dis_2;
  diffuse += Kd * base_color.rgb / PI * light.color * light.intensity * nl / dis_2;
  // SECTION

  vec3 acc = diffuse + specular;
  // vec3 color = acc * li
  FragColor = vec4(acc, 1.0);
}