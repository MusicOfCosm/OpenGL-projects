#version 460 core

out vec4 outColor;

in vec2 TexCoords;

uniform sampler2D renderTex;

void main()
{
	outColor = texture(renderTex, TexCoords);
}