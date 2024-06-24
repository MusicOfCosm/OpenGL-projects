#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in float aColor;
layout (location = 2) in vec3 aChange;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float scale;

out vec3 position;
out float color;

void main()
{
	color = aColor;
	gl_Position = projection * view * model * vec4(scale * .07 * aPosition + aChange, 1.0);
}