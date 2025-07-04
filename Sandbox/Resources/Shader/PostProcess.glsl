#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

struct VertexOutput 
{
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    Output.TexCoord = a_TexCoord;
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 FragmentColor;

struct VertexOutput 
{
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;


layout(std140, binding = 5) uniform Uniform {

    float Exposure;

} u_Uniform;

layout(binding = 0) uniform sampler2D u_Texture;

vec3 ReinhardTonemap(vec3 hdr, float whitePoint) {

    vec3 mapped = hdr * (1.0 + hdr / (whitePoint * whitePoint));
    return mapped / (1.0 + mapped);
}

vec3 ACESFittedTonemap(vec3 x) {
    const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
    return clamp((x*(A*x + B))/(x*(C*x + D) + E), 0.0, 1.0);
}

//vec3 Uncharted2Tonemap(vec3 x) {
//    float W = 11.2; // white point
//    vec3 curr = ((x*(0.15*x + 0.50) + 0.10)/(x*(0.15*x + 0.50) + 0.10)) - 0.02;
//    vec3 white = ((W*(0.15*W + 0.50) + 0.10)/(W*(0.15*W + 0.50) + 0.10)) - 0.02;
//    return curr / white;
//}

vec3 GammaCorrect(vec3 col) {

    return pow(col, vec3(1.0 / 2.2)); 
}


void main() {


    vec3 hdrColor = texture(u_Texture, Input.TexCoord).rgb;
    vec3 exposed = hdrColor * u_Uniform.Exposure;

    vec3 tonemapped = ReinhardTonemap(exposed, 1.0);
    vec3 srgb = GammaCorrect(tonemapped);

    FragmentColor = vec4(srgb, 1.0);
}