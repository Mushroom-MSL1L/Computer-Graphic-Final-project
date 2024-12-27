#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// TODO 4-1
// Implement CubeMap shading

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;


void main()
{
    TexCoords = aPos;

    vec4 Pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);

    gl_Position = Pos.xyww;


}