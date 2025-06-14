#     '                 GLSL.std.450                     main                   
    Resources/Shader/FSQuad.glsl     s    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
#version 450 core

layout(location = 0) out vec4 FragmentColor;

layout(location = 0) in VertexData {
    vec2 v_TexCoords;
};

layout(binding = 0) uniform sampler2D u_Texture;

void main() {
	
	float d = texture(u_Texture, v_TexCoords).r;
    FragmentColor = vec4(vec3(d), 1.0);
}   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   d        u_Texture        VertexData           v_TexCoords               FragmentColor   G     !       G     "       G        G            G                 !                              	 
                                 
                ;                                           ;                       +                                                +                        ;                       +     "     �?           6               �     ;     	                  =           A              =           W              Q               >  	                  =        	   P      !            Q     #   !       Q     $   !      Q     %   !      P     &   #   $   %   "   >     &               �  8  