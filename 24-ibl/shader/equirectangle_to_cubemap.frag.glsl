#version 450 core

uniform sampler2D tex;

in VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
}
fs_in;

out vec4 FragColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}

void main() {
  vec2 uv = SampleSphericalMap(normalize(fs_in.P));  // make sure to normalize localPos
  vec3 color = texture(tex, uv).rgb;
  FragColor = vec4(color, 1.0);
}