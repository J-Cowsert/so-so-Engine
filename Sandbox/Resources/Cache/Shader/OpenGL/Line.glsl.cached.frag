#                      GLSL.std.450                     main    
               	    Resources/Shader/Line.glsl   e    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
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
         
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   FragmentColor        VertexOutput             Color        Input   G  
          G                 !                               	         ;  	   
                          ;                       +                                   6               �                 A              =           >  
                  �  8  