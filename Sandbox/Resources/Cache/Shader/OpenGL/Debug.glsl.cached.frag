#     !                 GLSL.std.450                     main    
               	    Resources/Shader/Debug.glsl  p    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
#version 450 core

layout(location = 0) out vec4 Output;

layout(location = 0) in VertexData {

    vec3 v_Position;
    vec2 v_TexCoord;
};

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
    Output = vec4(v_TexCoord.x, v_TexCoord.y, 0., 1.0);
}
        
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   Output       VertexData           v_Position          v_TexCoord                 u_Texture   G  
          G        G            G      !       G      "            !                               	         ;  	   
                                                   ;                       +                        +                        +           +            +          �? 	                                                  ;                        6               �                 A                 =           A                 =           P                    >  
                  �  8  