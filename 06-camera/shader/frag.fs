#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

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
	FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), 0.2);
	// FragColor = texture(Texture1, TexCoord);
}
