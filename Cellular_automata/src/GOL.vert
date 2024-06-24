R"(
#version 410

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;


void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
)"