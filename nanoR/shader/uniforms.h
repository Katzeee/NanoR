layout(std140, binding = 0) uniform matrices {
  mat4 view;
  mat4 proj;
};

layout(std140, binding = 1) uniform camera {
  vec3 ws_cam_pos;
};

layout(std140, binding = 2) uniform per_obj {
  mat4 model;
};