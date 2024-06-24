#version 460 core

out vec4 outColor;

in vec2 TexCoords;

uniform sampler2D compTex;

void main()
{
	vec3 texel = texture(compTex, TexCoords).rgb;
	outColor = vec4(texel, 1.0);
}