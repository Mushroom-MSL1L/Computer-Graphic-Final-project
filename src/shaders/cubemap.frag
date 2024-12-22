#version 330 core
out vec4 FragColor ;

in vec3 textureCorrdinate ;
uniform samplerCube cubemap ; 

// TODO 4-1
// Implement CubeMap shading
void main()
{
    FragColor = texture(cubemap, textureCorrdinate) ;
}