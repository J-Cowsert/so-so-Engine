
#stage vertex
#version 450 core

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
    vec4 FragLightPosition; // for shadows. TODO: rename
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

//=================================================================================================

#stage fragment
#version 450 core

#define PI 3.14159265359

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

layout(location = 0) out vec4 FragmentColor;


layout(std140, binding = 2) uniform DirLight
{
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

} u_DirLight;

layout(std140, binding = 3) uniform Material
{
    vec3 DiffuseColor;
    vec3 SpecularColor;
    float Shininess;
    float Emission;
    
    bool HasNormalMap;

} u_Material;

layout(binding = 0) uniform sampler2D u_Diffuse;
layout(binding = 1) uniform sampler2D u_Specular;	
layout(binding = 2) uniform sampler2D u_Normal;

layout(binding = 3) uniform sampler2D u_ShadowMap;

void main() {

    // === standard vectors ===
    vec3  N  = normalize(Input.Normal);
    
    if (u_Material.HasNormalMap) 
    {
        N = normalize(2. * texture(u_Normal, Input.TexCoord).rgb - 1.);
        N = normalize(Input.TangentBasis * N);
    }

    vec3  L  = normalize(-u_DirLight.Direction.xyz);
    vec3  V  = normalize(Input.CameraPos - Input.WorldPosition);
    vec3  H  = normalize(L + V);

    // === base surface ===
    vec3  albedo   = texture(u_Diffuse, Input.TexCoord).rgb * u_Material.DiffuseColor.rgb;

    // === lighting ===
    vec3 ambient  = u_DirLight.Ambient.xyz  * albedo;

    float diff    = max(dot(N, L), 0.0);
    vec3 diffuse  = u_DirLight.Diffuse.xyz * diff * albedo;

    float specPow = pow(max(dot(N, H), 0.0), u_Material.Shininess) * (u_Material.Shininess + 2) / 2 * PI;
    vec3  specTint = texture(u_Specular, Input.TexCoord).rgb * u_Material.SpecularColor.rgb / 4. * PI;
    vec3 specular = u_DirLight.Specular.xyz * specPow * specTint;


    // === Shadows ===

    float shadow = 0.;
    vec3 lightCoord = Input.FragLightPosition.xyz / Input.FragLightPosition.w;
    if (lightCoord.z <= 1.) {

        lightCoord = (lightCoord + 1.) / 2.;

        float closestDepth = texture(u_ShadowMap, lightCoord.xy).r;
		
        float currDepth = lightCoord.z;

		//float closestDepth = texture(u_ShadowMap, vec3(lightCoord.xy, currDepth));

		float bias = max(0.005 * (1.0 - dot(N, L)), 0.0005);

        if (currDepth - bias > closestDepth) {

            shadow = 1.;
        }
    }


    vec3 result = ambient + (diffuse / PI) * (1. - shadow) + specular * (1. - shadow);


    FragmentColor = vec4(result, 1.);
}
