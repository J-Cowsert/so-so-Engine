#     3                 GLSL.std.450                      main           *   +   ,   /   1   2        Resources/Shader/DirectionalShadowMap.glsl   �    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
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

    
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               ShadowLightView  	        u_LightViewProjection                 Transform            u_Transform                a_Position    *   a_Normal      +   a_Tangent     ,   a_Bitangent   /   a_TexCoord    1   gl_VertexID   2   gl_InstanceID   G        H                H              H              H              G        H            H               H         #       G     !      G     "       G        H            H               H         #       G     !      G     "       G             G  *         G  +         G  ,         G  /         G  1         G  2              !                              	           +  	   
              
                                ;                       +                                           ;                                           ;                                  ;            +     "     �?   (         ;     *      ;     +      ;     ,        -            .      -   ;  .   /         0         ;  0   1      ;  0   2                 6               �                 A              =           A              =           �              =     !       Q     #   !       Q     $   !      Q     %   !      P     &   #   $   %   "   �     '      &   A  (   )         >  )   '               �  8  