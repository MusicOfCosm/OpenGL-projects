#version 460 core

out vec4 outColor;

in vec3 position;
in float color;
vec3 pos = position;

uniform vec3 fastColor;
uniform vec3 slowColor;

uniform float colorGradientSpeed;

vec3 result;

void main()
{
	if (color >= colorGradientSpeed)
		result = fastColor;
	else {
		float percent = color/colorGradientSpeed;
		result = slowColor + percent * (fastColor-slowColor);
	}

	outColor = vec4(result, 1.0);
}