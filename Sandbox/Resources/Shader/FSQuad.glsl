#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    mat4 u_RotationOnlyViewProjection;
	vec4 u_CameraPos;
};

layout(location = 0) out VertexData {
    vec2 v_TexCoords;
};

void main()
{
 
    v_TexCoords = a_TexCoord;   
    gl_Position = vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 FragmentColor;

layout(location = 0) in VertexData {
    vec2 v_TexCoords;
};

layout(binding = 0) uniform sampler2D u_Texture;

void main() {
	
	float d = texture(u_Texture, v_TexCoords).r;
    FragmentColor = vec4(vec3(d), 1.0);
}