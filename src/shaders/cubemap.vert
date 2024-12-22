#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 textureCorrdinate ;

uniform mat4 view;
uniform mat4 projection;

// TODO 4-1
// Implement CubeMap shading
void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0) ;
    gl_Position = gl_Position.xyww ;
    
    textureCorrdinate = aPos ;
}