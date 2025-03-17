#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;



void main()
{
 
    gl_Position = vec4(a_Position.xy, 0.0f, 1.0f); 
}

#type fragment
#version 450 core

out vec4 fragColor;

uniform vec3 iResolution;
uniform float iTime;
uniform vec4 iMouse;


void main() {

  vec2 uv = gl_FragCoord.xy/iResolution.xy;
  //uv.x *= iResolution.x/iResolution.y;
  vec3 color = vec3(0.950,1.0,0.80);
  float d = 0.0;

  // Remap the space to -1 to 1
  uv = uv * 2.0 - 1.0;

  // Make the distance field
  d = length( abs(uv)-0.216 / 10. ) * (iTime / -0.456);
  

  // Visualize the distance field
  fragColor = vec4(vec3(fract(d*10.0)) * color,abs((iTime/1.816)) + 0.208);

}	