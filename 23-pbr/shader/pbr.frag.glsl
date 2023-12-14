#version 450 core

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

struct PointLight {
  float intensity;
  vec3 ws_position;
  vec3 color;
};

uniform vec3 ws_cam_pos;

uniform vec3 albedo;
uniform float metallic = 0;
uniform float roughness = 1;
uniform float ao;
uniform PointLight p_lights[4];

const float PI = 3.14159265359;

out vec4 FragColor;

// clamp to avoid black points
vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1 - F0) * pow(clamp(1 - cosTheta, 0.0, 1.0), 5); }

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a_square = a * a;
  float nh = max(dot(N, H), 0.0);
  float denom = (nh * nh) * (a_square - 1) + 1;
  denom = PI * denom * denom;
  return a_square / denom;
}

float GeometrySchlickGGX(vec3 N, vec3 D, float roughness) {
  float nd = max(dot(N, D), 0.0);
  float k = (roughness + 1) * (roughness + 1) / 8;
  float denom = nd * (1 - k) + k;
  return nd / denom;
}

float GeomerytSmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float G1nl = GeometrySchlickGGX(N, L, roughness);
  float G1nv = GeometrySchlickGGX(N, V, roughness);
  return G1nl * G1nv;
}

void main() {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);
  vec3 N = normalize(fs_in.N);
  vec3 V = normalize(ws_cam_pos - fs_in.P);
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);
  for (int i = 0; i < 2; i++) {
    PointLight light = p_lights[i];
    vec3 L = light.ws_position - fs_in.P;
    float attennuation = 1 / length(L) / length(L);
    // float attennuation = 1;
    L = normalize(L);
    vec3 H = normalize(L + V);
    float nl = max(dot(N, L), 0.0);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3 Ks = F;
    vec3 Kd = vec3(1.0) - Ks;
    Kd *= 1.0 - metallic;
    float D = DistributionGGX(N, H, roughness);
    float G = GeomerytSmith(N, V, L, roughness);
    // float D = 1;
    // float G = 1;
    vec3 nom = D * G * F;
    // avoid devide 0
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    specular += nom / denom * light.color * light.intensity * nl * attennuation;
    diffuse += Kd * albedo / PI * light.color * light.intensity * nl * attennuation;
  }

  FragColor = vec4(specular + diffuse, 1);
}