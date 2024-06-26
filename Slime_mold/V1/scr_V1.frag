#version 460 core

out vec4 outColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
//uniform sampler2D agentData;

//float random(vec2 st) {
//    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
//}

uniform bool blur;
uniform int width; uniform int height;
float xoffset = 1.0 / width;
float yoffset = 1.0 / height;

vec2 offsets[9] = vec2[] (
    vec2(-xoffset,  yoffset), // top-left
    vec2( 0.0f,    yoffset), // top-center
    vec2( xoffset,  yoffset), // top-right
    vec2(-xoffset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( xoffset,  0.0f),   // center-right
    vec2(-xoffset, -yoffset), // bottom-left
    vec2( 0.0f,   -yoffset), // bottom-center
    vec2( xoffset, -yoffset)  // bottom-right    
);

void main()
{
    vec3 col = vec3(0.0);
    if (blur) {
        float kernel[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
    
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i]));
        }
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
    }
    else
        col = vec3(texture(screenTexture, TexCoords.st + offsets[4]));
    col -= 1/123.99999999999999;

    outColor = vec4(col, 1.0);

    //vec3 data = vec3(texture(agentData, TexCoords));
    //if (data.x > 100)
    //    outColor = vec4(1.0);
    //else
    //    outColor = vec4(vec3(0.0), 1.0);
    //outColor = vec4(data, 1.0);
}