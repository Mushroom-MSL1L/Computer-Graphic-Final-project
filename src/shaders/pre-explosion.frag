#version 330 core
out vec4 FragColor;

in vec2 TexCoord;    // Matches Geometry Shader
in vec3 FragNormal;  // Matches Geometry Shader
in vec3 FragPosition; // Matches Geometry Shader
in float isSpark;
in vec3 sparkColor;
in float isEffect;
in vec3 effectColor;

uniform sampler2D ourTexture ;
uniform samplerCube cubemap ;
uniform vec3 camera_position ;
uniform float material_gloss ;
uniform vec3 light_ambient ;
uniform vec3 light_diffuse ;
uniform vec3 light_position ;
uniform vec3 light_specular ;
uniform float time;
uniform float crackStartTime;
uniform float crackDuration;

void main()
{
    vec4 originalTexture = texture(ourTexture, TexCoord) ;
    float bias = 0.1;
    float alpha = 0.8 ; 
    float light_intensity = 1.0;

    if(isEffect == 1.0){
        FragColor = vec4(effectColor, 1.0);
        return;
    }
    else if(isEffect > 0.0){
        vec4 textureColor = texture(ourTexture, TexCoord) ;
        vec3 light_direction = normalize(light_position - FragPosition) ;
        vec3 view_direction = normalize(camera_position - FragPosition) ;

        vec3 Ambient = light_ambient ;
        vec3 Diffuse = light_diffuse * max(dot(light_direction, FragNormal), 0.0) ;
        
        vec3 H = normalize(light_direction + view_direction) ;
        vec3 Specular = light_specular * pow(max(dot(FragNormal, H), 0.0), material_gloss) ;

        vec3 light =  Ambient + Diffuse + Specular ; 
        vec3 result = light * effectColor ;
	    FragColor = vec4(result, 1.0) ;
        return;
    }
    if (isSpark > 0.0) {
        float glow = isSpark * 2.0;
        FragColor = vec4(sparkColor * glow, 1.0);
        return;
    }

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
