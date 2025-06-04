#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

layout(std140, binding = 4) uniform ShadowLightView {
    mat4 u_LightViewProjection;
};

layout(std140, binding = 1) uniform Transform {
    mat4 u_Transform;
};

void main() {

    gl_Position = u_LightViewProjection * u_Transform * vec4(a_Position, 1.);
}

#stage fragment
#version 450 core

void main() {

}