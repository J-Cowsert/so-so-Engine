
// Converts an equirectangular texture into a texture cube

// An equirectangular (latitude–longitude) projection texture is a way to represent the surface of a sphere on a flat, rectangular image. 
// It does so by “unwrapping” the sphere using its angular coordinates, longitude (horizontal angle) and latitude (vertical angle),
// and mapping them linearly to texture coordinates.  

//https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates

#stage compute
#version 450 core

const float PI = 3.141592;

layout(binding = 0) uniform sampler2D u_InputTexture;
layout(binding = 1, rgba16f) restrict writeonly uniform imageCube u_OutputTexture;

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main()
{

    vec2 st = gl_GlobalInvocationID.xy/vec2(imageSize(u_OutputTexture));
    vec2 uv = 2.0 * vec2(st.x, 1.0-st.y) - vec2(1.0);

    vec3 faceDirection;
	
    if(gl_GlobalInvocationID.z == 0)      faceDirection = vec3(1.0,  uv.y, -uv.x);
    else if(gl_GlobalInvocationID.z == 1) faceDirection = vec3(-1.0, uv.y,  uv.x);
    else if(gl_GlobalInvocationID.z == 2) faceDirection = vec3(uv.x, 1.0, -uv.y);
    else if(gl_GlobalInvocationID.z == 3) faceDirection = vec3(uv.x, -1.0, uv.y);
    else if(gl_GlobalInvocationID.z == 4) faceDirection = vec3(uv.x, uv.y, 1.0);
    else if(gl_GlobalInvocationID.z == 5) faceDirection = vec3(-uv.x, uv.y, -1.0);
     
    faceDirection = normalize(faceDirection);

    float phi = atan(faceDirection.z, faceDirection.x); // longitude  [-PI, PI]
    float theta = acos(faceDirection.y);				// colatitude (0 at top pole → PI at bottom) [0, PI]

    float u = phi / (2 * PI) + .5; // [-0.5, 0.5] -> [0, 1]
    float v = theta / PI;
    vec2 latLongUV = vec2(u,v);

    vec4 color = texture(u_InputTexture, latLongUV);

    imageStore(u_OutputTexture, ivec3(gl_GlobalInvocationID), color);
}