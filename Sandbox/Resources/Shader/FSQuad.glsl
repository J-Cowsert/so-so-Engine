#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

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
   
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragmentColor;

layout(location = 0) in VertexData {
    vec3 v_TexCoords;
};

layout(binding = 0) uniform sampler2D u_Texture;

void main() {
	
	discard;
    FragmentColor = vec4(1.0, 1., 0., 1.);
}