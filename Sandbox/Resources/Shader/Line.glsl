#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    mat4 u_RotationOnlyViewProjection;
};

struct VertexOutput 
{
    vec4 Color;
};

layout(location = 0) out VertexOutput Output;

void main()
{   
    Output.Color = a_Color;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

//=================================================================================================

#stage fragment
#version 450 core

layout(location = 0) out vec4 FragmentColor;

struct VertexOutput 
{
    vec4 Color;
};  

layout(location = 0) in VertexOutput Input;

void main()
{
    FragmentColor = Input.Color;
}
    