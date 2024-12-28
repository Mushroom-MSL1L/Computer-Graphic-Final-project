#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int shake;
uniform float u_time;

void main()
{
	vec3 newPos = aPos;
	if(shake == 1)
	{
		
        float offset = sin(u_time * 50.0) * 0.005;
		vec3 offsets = vec3(offset,offset,offset);
		newPos = aPos + offsets;
	}

    FragPos = vec3(model * vec4(newPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
