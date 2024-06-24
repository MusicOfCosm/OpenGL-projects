#version 410 core

out vec4 outColor;

uniform int width;
uniform int height;

uniform double xpos;
uniform double ypos;

// uniform sampler2D compTex;

// vec2 computeNext(vec2 current, vec2 constant)
// {
// 	//Zn�
// 	const float zr = current.x * current.x - current.y * current.y;
// 	const float zi = 2.0 * current.x * current.y;

// 	//Add constant
// 	return vec2(zr, zi) + constant;
// }

// int computeIterations(vec2 z0, vec2 constant, int max_iteration)
// {
// 	vec2 zn = z0;
// 	int iteration = 0;
// 	while (zn.x*zn.x + zn.y*zn.y < 4.0) {
// 		zn = computeNext(zn, constant);
// 		iteration++;
// 	}
// 	return iteration;
// }

vec2 xbounds = vec2(-2, 2); //ori:    -2, 0.47
vec2 ybounds = vec2(-2, 2); //ori: -1.12, 1.12
void bind(inout double x, inout double y)
{
	x = x / width;
	x = (((x - 0) * (xbounds.y - xbounds.x)) / (1 - 0)) + (xbounds.x);
	y = y / height;
	y = (((y - 0) * (ybounds.y - ybounds.x)) / (1 - 0)) + (ybounds.x);
}

void main()
{

	// outColor = texture(compTex, TexCoords);
	// float x0 = gl_FragCoord.x / width;
	// x0 = (((x0 - 0) * (xbounds.y - xbounds.x)) / (1 - 0)) + (xbounds.x);
	// float y0 = gl_FragCoord.y / height;
	// y0 = (((y0 - 0) * (ybounds.y - ybounds.x)) / (1 - 0)) + (ybounds.x);
	double x0 = gl_FragCoord.x; double y0 = gl_FragCoord.y;
	bind(x0, y0);
	dvec2 coords = dvec2(xpos, ypos);
	bind(coords.x, coords.y);

	double x = x0;
	double y = y0;

	int iter = 0;
	int max_iter = 100;
	while (x*x + y*y <= 4 && iter < max_iter) {
	    double xtemp = x;
        // x = x*x - y*y + x0;
        // y = 2*xtemp*y + y0;
        x = x*x - y*y + xpos;
        y = 2*xtemp*y + ypos;
        iter++;
	}

	vec3 color = vec3(1);
	// vec3 color = vec3(0.21f, 0.57f, 0.72f);
	// outColor = vec4(1)
    if (iter == max_iter) 
        outColor = vec4(vec3(0), 1);
    else
	    outColor = vec4(vec3(float(iter)/max_iter)*color, 1);
}

