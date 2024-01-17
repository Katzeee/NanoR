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

layout(std140, binding = 0) uniform _engine {
  mat4 view;
  mat4 proj;
  mat4 model;
  vec3 ws_cam_pos;
};

layout(std140, binding = 3) uniform pbr {
  vec4 base_color;
  vec4 test;
  vec3 test2;
};