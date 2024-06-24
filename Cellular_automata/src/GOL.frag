R"(
#version 410

out vec4 outColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;


float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}


uniform bool first;
uniform float width, height;

float xoffset = 1.0 / width;
float yoffset = 1.0 / height;

vec2 offsets[9] = vec2[](
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

float rule() {
    int neighbor = 0;
    bool alive = false;
    for (int i = 0; i < 9; i++)
    {
        if (i == 4 && vec3(texture(screenTexture, TexCoords.st + offsets[i])).x == 1.0)
            alive = true;
       
        else if (vec3(texture(screenTexture, TexCoords.st + offsets[i])).x == 1.0)
            neighbor += 1;
    }

    // Any live cell with two or three live neighbours survives.
    // Any dead cell with three live neighbours becomes a live cell.
    // All other live cells die in the next generation. Similarly, all other dead cells stay dead.
    if (alive && neighbor == 2 || alive && neighbor == 3) {
        return 1.0;
    }
    else if (!alive && neighbor == 3) {
        return 1.0;
    }
    else 
        return 0.0;
}

void main()
{
    float bw = random(TexCoords.st);
    if (first) {
        if (bw >= 0.5)
            outColor = vec4(1.0);
        else
            outColor = vec4(vec3(0.0), 1.0);
    }
    else {
        outColor = vec4(vec3(rule()), 1.0);
    }
}

/*#version 460 core

out vec4 outColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 1920.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    col -= 1/123.99999999999999;

    outColor = vec4(col, 1.0);
}*/
)"