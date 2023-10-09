#version 450 core

struct PointLight {
  float intensity;
  vec3 ws_position;
  vec3 color;
};

struct DirectLight {
  float intensity;
  vec3 direction;
  vec3 color;
};

in vec3 P;
in vec3 N;
in vec2 uv;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform vec3 ws_cam_pos;
uniform PointLight p_lights[2];
uniform DirectLight d_lights[1];
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

out vec4 FragColor;

float near = 0.1;
float far = 100;

float LinearizeDepth(float depth) {
  float z = depth * 2 - 1;  // to NDC
  return (2 * near * far) / (far + near - z * (far - near));
}

void main() {
  vec3 diffuse = vec3(0);
  vec3 specular = vec3(0);
  // PointLight
  for (int i = 0; i < 2; i++) {
    PointLight p_light = p_lights[i];
    vec3 L = p_light.ws_position - P;
    float distance_square = dot(L, L);
    L = normalize(L);
    vec3 V = normalize(ws_cam_pos - P);
    vec3 R = reflect(-L, N);
    diffuse += Kd * p_light.intensity * max(dot(L, N), 0) * texture(texture_diffuse0, uv).xyz * p_light.color /
               distance_square;
    specular += Ks * clamp(dot(N, L), 0, 1) * p_light.intensity * p_light.color *
                pow(max(dot(R, V), 0), texture(texture_specular0, uv).a) * texture(texture_specular0, uv).xyz /
                distance_square;
  }
  // DirectLight
  for (int i = 0; i < 1; i++) {
    DirectLight d_light = d_lights[i];
    vec3 L = d_light.direction;
    L = normalize(L);
    vec3 V = normalize(ws_cam_pos - P);
    vec3 R = reflect(-L, N);
    diffuse += Kd * d_light.intensity * max(dot(L, N), 0) * texture(texture_diffuse0, uv).xyz * d_light.color;
    specular += Ks * clamp(dot(N, L), 0, 1) * d_light.intensity * d_light.color *
                pow(max(dot(R, V), 0), texture(texture_specular0, uv).a) * texture(texture_specular0, uv).xyz;
  }

  vec3 ambient = Ka * texture(texture_diffuse0, uv).xyz;
#ifdef DEBUG_DEPTH
  FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z)) / far, 1);
#elif defined(DEBUG_NORMAL)
  FragColor = vec4(N, 1);
#else
  FragColor = vec4(diffuse + specular + ambient, 1);
#endif
}
