struct PointLight {
  vec3 color;
  float intensity;
  vec3 ws_position;
  float _padding;
};

struct DirectLight {
  vec3 color;
  float intensity;
  vec3 direction;
  float _padding;
};

layout(std140, binding = 0) uniform _engine {
  mat4 view;
  mat4 proj;
  mat4 model;
  vec3 ws_cam_pos;
};

layout(std140, binding = 1) uniform _light {
  PointLight p_light;
  // DirectLight d_light;
};

layout(std140, binding = 3) uniform pbr {
  vec4 base_color;
  float metallic;
  float roughness;
};