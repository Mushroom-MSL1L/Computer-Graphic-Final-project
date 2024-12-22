#version 330 core
out vec4 FragColor ;

in vec2 TexCoord;
in vec3 FragNormal ; 
in vec3 FragPosition ;

uniform sampler2D ourTexture ;
uniform vec3 rainbowColor ;

uniform vec3 camera_position ;
uniform float material_gloss ;
uniform vec3 light_ambient ;
uniform vec3 light_diffuse ;
uniform vec3 light_position ;
uniform vec3 light_specular ;

// TODO 2
// Implement Bling-Phong shading
void main()
{
    vec4 textureColor = texture(ourTexture, TexCoord) ;
	vec3 light_direction = normalize(light_position - FragPosition) ;
	vec3 view_direction = normalize(camera_position - FragPosition) ;

	vec3 Ambient = light_ambient ;
	vec3 Diffuse = light_diffuse * max(dot(light_direction, FragNormal), 0.0) ;
	
	vec3 H = normalize(light_direction + view_direction) ;
	vec3 Specular = light_specular * pow(max(dot(FragNormal, H), 0.0), material_gloss) ;

	vec3 light =  Ambient + Diffuse + Specular ; 
	vec3 result = light * textureColor.rgb ;
	FragColor = vec4(result, textureColor.a) ;
}