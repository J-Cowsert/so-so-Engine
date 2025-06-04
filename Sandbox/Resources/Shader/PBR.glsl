// PBR 

#stage vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera 
{
    mat4 u_ViewProjection;
    mat4 u_RotationOnlyViewProjection;
    vec4 u_CameraPos;
};

layout(std140, binding = 4) uniform ShadowLightView {
    mat4 u_LightViewProjection;
};

layout(std140, binding = 1) uniform Transform 
{
    mat4 u_Transform;
};

struct VertexOutput 
{
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TangentBasis;
    vec3 CameraPos;
    vec4 FragLightPosition;
};

layout(location = 0) out VertexOutput Output;

void main()
{   
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0)); 
    Output.TexCoord = a_TexCoord;
    Output.Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    Output.TangentBasis = mat3(u_Transform) * mat3(a_Tangent, a_Bitangent, a_Normal);
    Output.CameraPos = vec3(u_CameraPos);
    Output.FragLightPosition = u_LightViewProjection * (u_Transform * vec4(a_Position, 1.0));

    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}



#stage fragment
#version 440 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

layout(location = 0) out vec4 FragmentColor;

struct VertexOutput 
{
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TangentBasis;
    vec3 CameraPos;
    vec4 FragLightPosition;
};

layout(location = 0) in VertexOutput Input;


layout(std140, binding = 2) uniform DirLight
{
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

} u_DirLight;

layout(std140, binding = 3) uniform Material 
{
	vec3 BaseColor;
	float Metallic;
	float Roughness;

	bool HasNormalMap; // Consider making this a bitmask if more flags are needed

} u_Material;

layout(binding=0) uniform sampler2D u_BaseColorTexture;
layout(binding=1) uniform sampler2D u_NormalTexture;
layout(binding=2) uniform sampler2D u_MetalnessTexture;
layout(binding=3) uniform sampler2D u_RoughnessTexture;

layout(binding=4) uniform samplerCube u_EnvRadianceTexture;
layout(binding=5) uniform samplerCube u_EnvIrradianceTexture;
layout(binding=6) uniform sampler2D u_BRDF_LUT;
layout(binding=7) uniform sampler2D u_ShadowMap;


float SpecularD_GGX(float NdotH, float roughness) 
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

    float denom = (NdotH * NdotH) * (alphaSq - 1.) + 1.;
	return alphaSq / (PI * denom * denom);
}

float SpecularG_SchlickGGX(float NdotL, float NdotV, float roughness) 
{
    float k = ((roughness + 1.) * (roughness + 1.)) / 8.;

    float Gl = NdotL / (NdotL * (1. - k) + k);
    float Gv = NdotV / (NdotV * (1. - k) + k);

    return Gl * Gv;
}

vec3 SpecularF_FresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.) - F0) * pow(1. - cosTheta, 5.);
}

vec4 ToLinear(vec4 sRGB)
{
	bvec4 cutoff = lessThan(sRGB, vec4(0.04045));
	vec4 higher = pow((sRGB + vec4(0.055))/vec4(1.055), vec4(2.4));
	vec4 lower = sRGB/vec4(12.92);

	return mix(higher, lower, cutoff);
}

void main() 
{

	vec2 t_TileScale = vec2(1.);

	vec3 baseColor = texture(u_BaseColorTexture, Input.TexCoord * t_TileScale).rgb * vec3(ToLinear(vec4(u_Material.BaseColor, 1.)));
    float roughness = texture(u_RoughnessTexture, Input.TexCoord * t_TileScale).g * u_Material.Roughness;
	float metalness = texture(u_MetalnessTexture, Input.TexCoord * t_TileScale).b * u_Material.Metallic;	

	vec3  N  = normalize(Input.Normal);
    if (u_Material.HasNormalMap) 
    {
        N = normalize(2. * texture(u_NormalTexture, Input.TexCoord).rgb - 1.);
        N = normalize(Input.TangentBasis * N);
    }

	vec3 Lo = normalize(Input.CameraPos - Input.WorldPosition);
	

    vec3 F0 = mix(Fdielectric, baseColor, metalness);


	vec3 directLighting = vec3(0);
	for (int i = 0; i < 1; i++) 
	{
        vec3 Li = -u_DirLight.Direction; 
        vec3 Lh = normalize(Lo + Li);
		
		float NdotL = max(dot(N, Li), 0.);
        float NdotV = max(dot(N, Lo), 0.);
        float NdotH = max(dot(N, Lh), 0.);

        float D = SpecularD_GGX(NdotH, roughness);
        vec3 F = SpecularF_FresnelSchlick(F0, NdotH);
        float G = SpecularG_SchlickGGX(NdotL, NdotV, roughness);

        vec3 specularBRDF = (D * F * G) / max(4 * NdotL * NdotV, Epsilon);

        vec3 diffuseReflectance = mix(vec3(1.) - F, vec3(0), metalness);
        vec3 diffuseBRDF = diffuseReflectance * baseColor;

        vec3 Lradiance   = u_DirLight.Ambient;   
        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
	}
	

    vec3 ambiant = baseColor * vec3(u_DirLight.Diffuse.x);

    FragmentColor = vec4(directLighting + ambiant, 1.);
}
