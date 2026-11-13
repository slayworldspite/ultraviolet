#version 460 core

out vec4 Output;

in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;
uniform float u_Gamma;
uniform vec2 u_Resolution;
uniform bool u_EnableFXAA;

#define FXAA_SPAN_MAX	8.0
#define FXAA_REDUCE_MUL 1.0 / 8.0
#define FXAA_REDUCE_MIN 1.0 / 128.0

// https://www.4rknova.com/blog/2015/02/15/fxaa
vec3 fxaa(vec2 p);

void main()
{	
	vec4 color;

	if (u_EnableFXAA)
		color = vec4(fxaa(o_TextureCoordinates), 1.0);
	else
		color = texture(u_Framebuffer, o_TextureCoordinates);

	color.rgb = color.rgb / (color.rgb + vec3(1.0));
	color.rgb = pow(color.rgb, vec3(1.0 / u_Gamma));

	Output = color;
}

// https://www.4rknova.com/blog/2015/02/15/fxaa
vec3 fxaa(vec2 p)
{
	// 1st stage - Find edge
	vec3 rgbNW = texture(u_Framebuffer, p + (vec2(-1.,-1.) / u_Resolution)).rgb;
	vec3 rgbNE = texture(u_Framebuffer, p + (vec2( 1.,-1.) / u_Resolution)).rgb;
	vec3 rgbSW = texture(u_Framebuffer, p + (vec2(-1., 1.) / u_Resolution)).rgb;
	vec3 rgbSE = texture(u_Framebuffer, p + (vec2( 1., 1.) / u_Resolution)).rgb;
	vec3 rgbM = texture(u_Framebuffer, p).rgb;
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float lumaSum = lumaNW + lumaNE + lumaSW + lumaSE;
	float dirReduce = max(lumaSum * (0.25 * FXAA_REDUCE_MUL)
	                    , FXAA_REDUCE_MIN);
	float rcpDirMin = 1. / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX),
	            max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) / u_Resolution;
	 
	// 2nd stage - Blur
	vec3 rgbA = .5 * (texture(u_Framebuffer, p + dir * (1./3. - .5)).rgb +
		texture(u_Framebuffer, p + dir * (2./3. - .5)).rgb);
	vec3 rgbB = rgbA * .5 + .25 * (
			        texture(u_Framebuffer, p + dir * (0./3. - .5)).rgb +
	        		texture(u_Framebuffer, p + dir * (3./3. - .5)).rgb
	            );
	float lumaB = dot(rgbB, luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE),
	                                min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), 
	                                max(lumaSW, lumaSE)));
	return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;
}