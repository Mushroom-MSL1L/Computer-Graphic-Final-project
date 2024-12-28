#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec2 TexCoord;
    vec3 FragNormal;
    vec3 FragPosition;
    vec4 worldPosition;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.TexCoord = aTexCoord;
    vs_out.FragNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vs_out.FragPosition = vec3(model * vec4(aPos, 1.0));
    vs_out.worldPosition = model * vec4(aPos, 1.0);
}
