#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    mat4 u_RotationOnlyViewProjection;
};

layout(location = 0) out VertexData {
    vec3 v_TexCoords;
};

void main()
{
 
    v_TexCoords = a_Position;
   
    gl_Position = (u_RotationOnlyViewProjection * vec4(a_Position, 1.0)).xyww;
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 Output;

layout(location = 0) in VertexData {
    vec3 v_TexCoords;
};

layout(binding = 0) uniform samplerCube u_Skybox;

void main() {
    Output = texture(u_Skybox, v_TexCoords);
}	