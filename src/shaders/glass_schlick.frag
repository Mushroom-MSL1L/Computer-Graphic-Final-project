#version 330 core
out vec4 FragColor ;

in vec3 FragNormal ;
in vec3 FragPosition ;

uniform samplerCube cubemap ;
uniform vec3 light_position ;
uniform vec3 camera_position ;

// TODO 6-1:
// Implement Glass-Schlick shading
void main() 
{
    vec3 light_direction = normalize(FragPosition - camera_position) ;
    vec3 reflection = normalize(light_direction - 2 * dot(light_direction, FragNormal) * FragNormal) ;
    vec3 reflectionTexture = texture(cubemap, reflection).rgb ;

    float air_refractive_index = 1.0 ; 
    float metal_refractive_index = 1.52 ;

    float eta = air_refractive_index / metal_refractive_index ;
    float cosine = dot(light_direction, FragNormal) ;
    float k = 1 - eta * eta * (1 - pow(cosine, 2.0)) ; 
    vec3 refraction = vec3(0.0) ;
    if (k > 0) {
        refraction = eta * light_direction - (eta * (cosine) + sqrt(k)) * FragNormal ; 
    }
    vec3 refractionTexture = texture(cubemap, refraction).rgb ;

    float r0 = pow((air_refractive_index - metal_refractive_index) / (air_refractive_index + metal_refractive_index), 2.0) ;
    float r_theta = r0 + (1 - r0) * pow(1 + cosine, 5.0) ;
    FragColor = vec4(r_theta * reflectionTexture + (1 - r_theta) * refractionTexture, 1.0) ;
}	
