// Wireframe debug

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

layout(std140, binding = 1) uniform Transform 
{
    mat4 u_Transform;
};

void main()
{   
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}


//===============================================================================================


#stage geometry
#version 440 core

// We’ll take triangles from the vertex shader…
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

// Pass through a simple solid color for the wireframe
layout(location = 0) out vec4 gs_Color;

void main()
{
    // White wireframe
    vec4 color = vec4(1.0);

    // for each triangle edge (0→1, 1→2, 2→0)
    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;

        // emit start of edge
        gl_Position = gl_in[i].gl_Position;
        gs_Color    = color;
        EmitVertex();

        // emit end of edge
        gl_Position = gl_in[j].gl_Position;
        gs_Color    = color;
        EmitVertex();

        EndPrimitive();
    }
}


//===============================================================================================


#stage fragment

#version 440 core
layout(location = 0) in vec4 gs_Color;
layout(location = 0) out vec4 FragColor;

void main() 
{ 
    FragColor = gs_Color; 
}
