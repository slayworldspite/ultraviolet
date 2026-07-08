#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_NormalMap;
uniform sampler2D u_DepthMap;

layout (std140, binding = 0) uniform CameraData
{
	mat4 ub_Projection;
	mat4 ub_View;
	vec3 ub_CameraPosition;
};

const vec3 samples[64] = vec3[](
    vec3( 0.5381,  0.1856,  0.4319),
    vec3( 0.1379,  0.2486,  0.4430),
    vec3( 0.3371,  0.5679,  0.0057),
    vec3(-0.6999, -0.0451,  0.0019),
    vec3( 0.0689, -0.1598,  0.4547),
    vec3( 0.0560,  0.0069,  0.1843),
    vec3(-0.0146,  0.1402,  0.0762),
    vec3( 0.0100, -0.1924,  0.0344),

    vec3(-0.3577, -0.5301,  0.4358),
    vec3(-0.3169,  0.1063,  0.0158),
    vec3( 0.0103, -0.5869,  0.0046),
    vec3(-0.0897, -0.4940,  0.3287),
    vec3( 0.7119, -0.0154,  0.0918),
    vec3(-0.0533,  0.0596,  0.5411),
    vec3( 0.0350, -0.0631,  0.5460),
    vec3(-0.4776,  0.2847,  0.0271),

    vec3(-0.2147,  0.4921,  0.4528),
    vec3( 0.1546, -0.3527,  0.3194),
    vec3(-0.4415, -0.1163,  0.2017),
    vec3( 0.3422,  0.3127,  0.3921),
    vec3(-0.1215,  0.6218,  0.3125),
    vec3( 0.5238, -0.2114,  0.2769),
    vec3(-0.6317,  0.1512,  0.0984),
    vec3( 0.2145,  0.7321,  0.1342),

    vec3(-0.5124, -0.3721,  0.4125),
    vec3( 0.6231,  0.1047,  0.3388),
    vec3(-0.3417,  0.4215,  0.2219),
    vec3( 0.1123, -0.6724,  0.3841),
    vec3( 0.4528, -0.4927,  0.1734),
    vec3(-0.2841, -0.2316,  0.5332),
    vec3( 0.7135,  0.2314,  0.1128),
    vec3(-0.6214,  0.3489,  0.2947),

    vec3( 0.1832,  0.5127,  0.4361),
    vec3(-0.1735, -0.6138,  0.2674),
    vec3( 0.3649, -0.4172,  0.3586),
    vec3(-0.7352,  0.0831,  0.2046),
    vec3( 0.5824,  0.4237,  0.2981),
    vec3(-0.4126, -0.5582,  0.1463),
    vec3( 0.2961,  0.0914,  0.6428),
    vec3(-0.0824,  0.3765,  0.5119),

    vec3( 0.4763, -0.0617,  0.5234),
    vec3(-0.5517,  0.2674,  0.3521),
    vec3( 0.0732, -0.4821,  0.5842),
    vec3(-0.2938,  0.6452,  0.1124),
    vec3( 0.6418, -0.3185,  0.2026),
    vec3(-0.4462,  0.0327,  0.4937),
    vec3( 0.2187,  0.5931,  0.3612),
    vec3(-0.6635, -0.1824,  0.2517),

    vec3( 0.3924,  0.2146,  0.6123),
    vec3(-0.2142, -0.4418,  0.5326),
    vec3( 0.5427, -0.5372,  0.1049),
    vec3(-0.5821,  0.4963,  0.1872),
    vec3( 0.1098,  0.3045,  0.7214),
    vec3(-0.3764, -0.0875,  0.6198),
    vec3( 0.7021,  0.3726,  0.0921),
    vec3(-0.7113, -0.2954,  0.1638),

    vec3( 0.2576, -0.7284,  0.2147),
    vec3(-0.1396,  0.6872,  0.2845),
    vec3( 0.4871,  0.5573,  0.2456),
    vec3(-0.5234, -0.4231,  0.3189),
    vec3( 0.3185, -0.2487,  0.6074),
    vec3(-0.2678,  0.1764,  0.6841),
    vec3( 0.6114, -0.1092,  0.3928),
    vec3(-0.4827,  0.3618,  0.4482)
);

const vec3 noise[16] = vec3[](
    vec3(-0.44,  0.89, 0.0),
    vec3( 0.72, -0.31, 0.0),
    vec3(-0.91, -0.12, 0.0),
    vec3( 0.26,  0.96, 0.0),

    vec3( 0.57,  0.42, 0.0),
    vec3(-0.68,  0.73, 0.0),
    vec3( 0.15, -0.98, 0.0),
    vec3(-0.83,  0.36, 0.0),

    vec3( 0.91, -0.40, 0.0),
    vec3(-0.22, -0.84, 0.0),
    vec3( 0.49,  0.87, 0.0),
    vec3(-0.75, -0.54, 0.0),

    vec3( 0.34, -0.63, 0.0),
    vec3(-0.56,  0.18, 0.0),
    vec3( 0.81,  0.22, 0.0),
    vec3(-0.12,  0.71, 0.0)
);

vec3 GetPositionFromDepth(vec2 textureCoordinates, float depth)
{
    vec4 clip = vec4(
            textureCoordinates * 2.0 - 1.0,
            depth * 2.0 - 1.0,
            1.0
        );

        vec4 view = inverse(ub_Projection) * clip;

        return view.xyz / view.w;
}

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    float depth = texture(u_DepthMap, o_TextureCoordinates).r;

    vec3 normal = texture(u_NormalMap, o_TextureCoordinates).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(normal);

    vec3 position = GetPositionFromDepth(o_TextureCoordinates, depth);

    float occlusion = 0.0;

    vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);

    int x = int(mod(gl_FragCoord.x, 4.0));
    int y = int(mod(gl_FragCoord.y, 4.0));

    int noiseIndex = x + y * 4;

    vec3 randomVector = noise[noiseIndex];

    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);

    mat3 TBN = mat3(
        tangent,
        bitangent,
        normal
    );

    for (int i = 0; i < 64; i++)
    {
        vec3 samplePosition = position + (TBN * samples[i]) * 0.5;

        vec4 offset = ub_Projection * vec4(samplePosition, 1.0);
        offset.xyz /= offset.w;

        vec2 sampleUV = offset.xy * 0.5 + 0.5;
        
        float sampleDepth = texture(u_DepthMap, sampleUV).r;
        vec3 samplePositionDepth = GetPositionFromDepth(sampleUV, sampleDepth);

        if (samplePositionDepth.z >= samplePosition.z + 0.025)
            occlusion += 1.0;
    }

    occlusion = 1.0 - (occlusion / 64.0);

    Output = vec4(vec3(occlusion), 1.0);
}