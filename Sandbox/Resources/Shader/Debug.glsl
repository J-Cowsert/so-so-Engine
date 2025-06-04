#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    mat4 u_RotationOnlyViewProjection;
};

layout(std140, binding = 1) uniform Transform {
    mat4 u_Transform;
};

layout(location = 0) out VertexData {

    vec3 v_Position;
    vec2 v_TexCoord;
};

void main()
{   
    v_Position = vec3(u_Transform * vec4(a_Position, 1.0)); 
    
    v_TexCoord = a_TexCoord;

    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

//=================================================================================================

#stage fragment
#version 450 core

layout(location = 0) out vec4 Output;

layout(location = 0) in VertexData {

    vec3 v_Position;
    vec2 v_TexCoord;
};

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
    Output = vec4(v_TexCoord.x, v_TexCoord.y, 0., 1.0);
}
    