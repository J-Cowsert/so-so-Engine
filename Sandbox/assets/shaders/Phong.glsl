#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

uniform float u_Time;

out vec3 v_FragPos;       // Fragment position in world space
out vec3 v_Normal;        // Normal in world space
out vec2 v_TexCoord;      // Texture coordinates

void main()
{   
    //float dist = length(a_Position.xz);
    //float wobble = sin(u_Time * 2.0 + dist * 5.0) * 0.2; 
    //vec3 modifiedPosition = a_Position + vec3(0.0, wobble,cos(u_Time * 2.0 + dist * 5.0) * 0.2 );

    // Pass to fragment shader
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0)); 
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal; 
    v_TexCoord = a_TexCoord;

    // Final vertex position
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec3 v_FragPos;       // Fragment position in world space
in vec3 v_Normal;        // Normal in world space
in vec2 v_TexCoord;      // Texture coordinates

uniform sampler2D u_Texture;
uniform float u_TileScale;

// Lighting uniforms
uniform vec3 u_LightPos;      // Light position in world space
uniform vec3 u_ViewPos;       // Camera position in world space
uniform vec3 u_LightColor;    // Light color
uniform float u_AmbientStrength; // Ambient strength
uniform float u_SpecularStrength; // Specular strength
uniform float u_SpecularShininess; // Specular shininess
uniform float u_LightIntensity;  // Light intensity

void main()
{
    // Tile the texture
    vec2 tiledTexCoord = v_TexCoord * u_TileScale;
    vec4 texColor = texture(u_Texture, tiledTexCoord);

    // Normalize the normal
    vec3 norm = normalize(v_Normal);

    // Ambient lighting
    vec3 ambient = u_AmbientStrength * u_LightColor * u_LightIntensity;

    // Diffuse lighting
    vec3 lightDir = normalize(u_LightPos - v_FragPos); // Light direction
    float diff = max(dot(norm, lightDir), 0.0);       // Diffuse intensity
    vec3 diffuse = diff * u_LightColor * u_LightIntensity;

    // Specular lighting
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);  // View direction
    vec3 reflectDir = reflect(-lightDir, norm);       // Reflect direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_SpecularShininess); // Specular intensity
    vec3 specular = u_SpecularStrength * spec * u_LightColor * u_LightIntensity;

    // Combine all lighting components
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;

    // Output the final color
    color = vec4(result, texColor.a);
}
    