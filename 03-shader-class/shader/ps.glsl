#version 330 core
in vec3 color;
out vec4 FragColor;

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}
void main() { 
  FragColor = vec4(ACESToneMapping(color, 1.5f), 1.0f);
}
