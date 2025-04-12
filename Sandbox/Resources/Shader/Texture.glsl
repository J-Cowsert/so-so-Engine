// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{   
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0)); 
    v_TexCoord = a_TexCoord;
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;

    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

struct DirLight {
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    float     Shininess;
};

uniform DirLight u_DirLight;	
uniform Material u_Material;
uniform vec3     u_CamPos;

void main() {

	vec3 norm    = normalize(v_Normal);
    vec3 lightDir= normalize(-u_DirLight.Direction);
    vec3 viewDir = normalize(u_CamPos - v_FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    vec3 ambient  = u_DirLight.Ambient * texture(u_Material.Diffuse, v_TexCoord).rgb;
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = u_DirLight.Diffuse * diff * texture(u_Material.Diffuse, v_TexCoord).rgb;
    float spec    = pow(max(dot(norm, halfDir), 0.0), u_Material.Shininess);
    vec3 specular = u_DirLight.Specular * spec * texture(u_Material.Specular, v_TexCoord).rgb;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0);
}
