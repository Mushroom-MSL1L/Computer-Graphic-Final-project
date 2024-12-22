#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 VertexLight ; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_position ;
uniform float material_gloss ;
uniform vec3 light_ambient ;
uniform vec3 light_diffuse ;
uniform vec3 light_position ;
uniform vec3 light_specular ;

// TODO 3:
// Implement Gouraud shading
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;

    vec3 VertexNormal =  normalize(mat3(transpose(inverse(model))) * aNormal) ;
    vec3 VertexPosition = vec3(model * vec4(aPos, 1.0)) ; 
    vec3 light_direction = normalize(light_position - VertexPosition) ;
	vec3 view_direction = normalize(camera_position - VertexPosition) ;

	vec3 Ambient = light_ambient ;
	vec3 Diffuse = light_diffuse * max(dot(light_direction, VertexNormal), 0.0) ;
	vec3 H = normalize(light_direction + view_direction) ;
	vec3 Specular = light_specular * pow(max(dot(VertexNormal, H), 0.0), material_gloss) ;

	VertexLight = Ambient + Diffuse + Specular ; 
}