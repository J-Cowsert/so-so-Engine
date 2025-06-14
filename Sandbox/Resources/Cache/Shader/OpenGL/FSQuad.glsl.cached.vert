#     ,                 GLSL.std.450                      main                *   +    
    Resources/Shader/FSQuad.glsl     �    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
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

   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   VertexData    	       v_TexCoords               a_TexCoord       gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               a_Position    &   Camera    &       u_ViewProjection      &      u_RotationOnlyViewProjection      &      u_CameraPos   (         *   gl_VertexID   +   gl_InstanceID   G  	      G            G           G        H                H              H              H              G            G  &      H  &          H  &             H  &       #       H  &         H  &            H  &      #   @   H  &      #   �   G  (   !       G  (   "       G  *         G  +              !                              	         
      	   ;  
                     +                        ;                                               +                                                   ;                                  ;           +          �?   #           %           &   %   %         '      &   ;  '   (         )         ;  )   *      ;  )   +                 6               �                 =           A              >                    =           Q               Q               Q     !         P     "          !      A  #   $         >  $   "               �  8  