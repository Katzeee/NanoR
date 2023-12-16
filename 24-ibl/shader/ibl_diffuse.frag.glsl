#version 450 core

// sample normal
in vec3 uv;
const float PI = 3.14159265359;

uniform samplerCube tex;
out vec4 FragColor;

void main() {
  vec3 color = vec3(0.0);
  // 这里只需要构造出任意一个切线空间即可，因为不需要映射纹理
  vec3 up = vec3(0, 1, 0);
  vec3 N = normalize(uv);
  // bitangent
  vec3 right = normalize(cross(up, N));
  // tangent
  up = normalize(cross(N, right));
  int sample_count = 200;
  vec3 irradiance = vec3(0.0);
  for (int i = 0; i < sample_count; i++) {
    float theta = (PI / 2) * (float(i) / float(sample_count));
    for (int j = 0; j < sample_count; j++) {
      float phi = (2 * PI) * (float(j) / float(sample_count));
      // tangent space
      vec3 dir = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
      // world space
      vec3 sample_dir = dir.x * right + dir.y * up + dir.z * N;
      irradiance += texture(tex, sample_dir).rgb * cos(theta) * sin(theta);
    }
  }
  irradiance *= PI;
  irradiance /= sample_count * sample_count;

  // float sampleDelta = 0.025;
  // float nrSamples = 0.0;
  // for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
  //   for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
  //     // spherical to cartesian (in tangent space)
  //     vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  //     // tangent space to world
  //     vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

  //     irradiance += texture(tex, sampleVec).rgb * cos(theta) * sin(theta);
  //     nrSamples++;
  //   }
  // }
  // irradiance = PI * irradiance * (1.0 / float(nrSamples));

  FragColor = vec4(irradiance, 1.0);
}