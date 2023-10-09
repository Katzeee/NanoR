#version 450 core

struct PointLight {
  float intensity;
  vec3 ws_position;
  vec3 color;
};

in vec3 P;
in vec3 N;
in vec2 uv;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform vec3 ws_cam_pos;
uniform PointLight lights[2];
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
  for (int i = 0; i < 2; i++) {
    PointLight light = lights[i];
    vec3 L = light.ws_position - P;
    float distance_square = dot(L, L);
    L = normalize(L);
    vec3 V = normalize(ws_cam_pos - P);
    vec3 R = reflect(-L, N);
    diffuse +=
        Kd * light.intensity * max(dot(L, N), 0) * texture(texture_diffuse0, uv).xyz * light.color / distance_square;
    specular += Ks * clamp(dot(N, L), 0, 1) * light.intensity *
                pow(max(dot(R, V), 0), texture(texture_specular0, uv).a) * texture(texture_specular0, uv).xyz /
                distance_square;
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
