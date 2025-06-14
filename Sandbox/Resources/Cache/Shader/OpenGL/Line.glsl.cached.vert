#     +                 GLSL.std.450              	        main             "    	    Resources/Shader/Line.glsl   �    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
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

  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   VertexOutput      	       Color        Output       a_Color      gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               Camera           u_ViewProjection            u_RotationOnlyViewProjection               "   a_Position  G            G           G        H                H              H              H              G        H            H               H         #       H           H              H        #   @   G     !       G     "       G  "               !                              	         
      	   ;  
                     +                        ;                                    +                                                   ;                                             ;                                      !          ;  !   "      +     $     �?           6               �                 =           A              >                    A              =           =      #   "   Q     %   #       Q     &   #      Q     '   #      P     (   %   &   '   $   �     )      (   A     *         >  *   )               �  8  