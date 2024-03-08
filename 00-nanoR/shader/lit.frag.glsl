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

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

layout(std140, binding = 1) uniform vectors {
  uniform vec3 ws_cam_pos;
};

uniform vec4 base_color = vec4(1, 1, 1, 1);
uniform mat4 world_to_light_space_matrix;
uniform sampler2D albedo;
uniform sampler2D texture_specular0;
uniform sampler2D depth_map;



uniform PointLight p_lights[5];
uniform DirectLight d_lights[1];
uniform vec3 Ka = vec3(0.1);
uniform vec3 Kd = vec3(0.5);
uniform vec3 Ks;

out vec4 FragColor;

float near = 0.1;
float far = 150;

float LinearizeDepth(float depth) {
  float z = depth * 2 - 1;  // to NDC
  return (2 * near * far) / (far + near - z * (far - near));
}

float PCF(int size) {
  float kernel_area = (2 * size + 1) * (2 * size + 1);
  vec4 ls_P = world_to_light_space_matrix * vec4(fs_in.P, 1);
  ls_P = vec4(ls_P.xyz / ls_P.w, 1);
  ls_P = ls_P * 0.5 + 0.5;
  float cur_depth = ls_P.z;
  // NOTICE: must use 1.0 rather than 1
  vec2 texel_size = 1.0 / textureSize(depth_map, 0);
  vec3 L = normalize(d_lights[0].direction);
  vec3 N = normalize(fs_in.N);
  float bias = mix(0.000005, 0.00001, dot(L, N));
  float shadow = 0;
  for (int i = -size; i <= size; i++) {
    for (int j = -size; j <= size; j++) {
      float closet_depth = texture(depth_map, ls_P.xy + vec2(texel_size.x * i, texel_size.y * j)).r;
      shadow += cur_depth - bias > closet_depth ? 0 : 1.0;
    }
  }
  if (ls_P.z > 1) {  // exceed light's far plain
    shadow = kernel_area;
  }
  return shadow / kernel_area;  // make cur_depth closer to light
}

float PCSS(int size) {
  float kernel_area = (2 * size + 1) * (2 * size + 1);
  vec4 ls_P = world_to_light_space_matrix * vec4(fs_in.P, 1);
  ls_P = vec4(ls_P.xyz / ls_P.w, 1);
  ls_P = ls_P * 0.5 + 0.5;
  float cur_depth = ls_P.z;
  vec2 texel_size = 1.0 / textureSize(depth_map, 0);
  float mean_blocker_depth = 0.0;
  for (int i = -size; i <= size; i++) {
    for (int j = -size; j <= size; j++) {
      mean_blocker_depth += texture(depth_map, ls_P.xy + vec2(i, j) * texel_size).r;
    }
  }
  mean_blocker_depth /= kernel_area;
  float light_size = 20000.0;
  float pcf_size = (cur_depth - mean_blocker_depth) / mean_blocker_depth * light_size;
  return PCF(int(pcf_size));
}

void main() {
  // vec4 color = texture(albedo, fs_in.uv);
  // FragColor = vec4(base_color);
  // return;
  vec3 diffuse = vec3(0);
  vec3 specular = vec3(0);
  // PointLight
  for (int i = 0; i < 5; i++) {
    PointLight p_light = p_lights[i];
    vec3 L = p_light.ws_position - fs_in.P;
    float distance_square = dot(L, L);
    L = normalize(L);
    vec3 V = normalize(ws_cam_pos - fs_in.P);
    vec3 H = normalize(L + V);
    vec3 R = reflect(-L, fs_in.N);
    diffuse += Kd * p_light.intensity * max(dot(L, fs_in.N), 0) * texture(albedo, fs_in.uv).rgb * p_light.color /
               distance_square;
    // diffuse += texture(albedo, fs_in.uv).rgb * p_light.color * max(dot(L, fs_in.N), 0) * p_light.intensity;
    specular += Ks * clamp(dot(fs_in.N, L), 0, 1) * p_light.intensity * p_light.color *
#ifdef BLINN_PHONG
                pow(max(dot(H, fs_in.N), 0), texture(texture_specular0, fs_in.uv).a) *
#else
                pow(max(dot(V, R), 0), texture(texture_specular0, fs_in.uv).a) *
#endif
                texture(texture_specular0, fs_in.uv).rgb / distance_square;
  }
  // DirectLight
  for (int i = 0; i < 1; i++) {
    DirectLight d_light = d_lights[i];
    vec3 L = d_light.direction;
    L = normalize(L);
    vec3 V = normalize(ws_cam_pos - fs_in.P);
    vec3 R = reflect(-L, fs_in.N);
    diffuse += Kd * d_light.intensity * max(dot(L, fs_in.N), 0) * texture(albedo, fs_in.uv).rgb * d_light.color;
    specular += Ks * clamp(dot(fs_in.N, L), 0, 1) * d_light.intensity * d_light.color *
                pow(max(dot(R, V), 0), texture(texture_specular0, fs_in.uv).a) *
                texture(texture_specular0, fs_in.uv).rgb;
  }
  vec3 ambient = Ka * texture(albedo, fs_in.uv).rgb;
#ifdef DEBUG_DEPTH
  FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z)) / far, 1);
#elif defined(DEBUG_NORMAL)
  FragColor = vec4(fs_in.N, 1);
#else
  FragColor = base_color * vec4(((diffuse + specular) *
#ifdef PCF_SHADOW
                                     PCF(2)
#elif defined(PCSS_SHADOW)
                                     PCSS(3)
#else
                                     1.0
#endif
                                 + ambient),
                                texture(albedo, fs_in.uv).a);
#endif
}
