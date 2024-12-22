#version 330 core
out vec4 FragColor ;

in vec3 FragNormal ;
in vec3 FragPosition ;
in vec2 TexCoord ;

uniform sampler2D ourTexture ;
uniform samplerCube cubemap ;
uniform vec3 light_position ;
uniform vec3 camera_position ;

// TODO 5:
// Implement Metallic shading
void main() 
{
    vec4 originalTexture = texture(ourTexture, TexCoord) ;
    float bias = 0.2 ;
    float alpha = 0.4 ; 
    float light_intensity = 1 ;

    vec3 light_direction = normalize(FragPosition - camera_position) ;
    vec3 reflection = normalize(light_direction - 2 * dot(light_direction, FragNormal) * FragNormal) ;
    vec3 reflectionTexture = texture(cubemap, reflection).rgb ;

    float bidirectionReflection = max(dot(-light_direction, FragNormal) * light_intensity , 0.0) + bias ;
    FragColor = vec4(alpha * bidirectionReflection * originalTexture.rgb + (1.0 - alpha) * reflectionTexture, originalTexture.a) ;
}

