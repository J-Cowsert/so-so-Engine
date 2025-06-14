#                      GLSL.std.450                     main    
               
    Resources/Shader/Skybox.glsl     g    �     // OpModuleProcessed entry-point main
// OpModuleProcessed client opengl100
// OpModuleProcessed target-env opengl
// OpModuleProcessed entry-point main
#line 1
#version 450 core

layout(location = 0) out vec4 Output;

layout(location = 0) in VertexData {
    vec3 v_TexCoords;
};

layout(binding = 0) uniform samplerCube u_Skybox;

void main() {
    Output = texture(u_Skybox, v_TexCoords);
}	   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   Output       u_Skybox         VertexData           v_TexCoords          G  
          G     !       G     "       G        G                 !                               	         ;  	   
       	                                                  ;                                           ;                       +                                   6               �                 =           A              =           W              >  
                  �  8  