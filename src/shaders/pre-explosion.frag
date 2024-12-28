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
uniform vec3 light_position ;
uniform vec3 camera_position ;
uniform float time;

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
        vec3 light_direction = normalize(FragPosition - camera_position) ;
        vec3 reflection = normalize(light_direction - 2 * dot(light_direction, FragNormal) * FragNormal) ;
        vec3 reflectionTexture = texture(cubemap, reflection).rgb ;

        float bidirectionReflection = max(dot(-light_direction, FragNormal) * light_intensity , 0.0) + bias ;
        FragColor = vec4(alpha * bidirectionReflection * effectColor + (1.0 - alpha) * reflectionTexture, 1.0) ;
        return;
    }
    if (isSpark > 0.0) {
        float glow = isSpark * 2.0;
        FragColor = vec4(sparkColor * glow, 1.0);
        return;
    }

    vec3 light_direction = normalize(FragPosition - camera_position) ;
    vec3 reflection = normalize(light_direction - 2 * dot(light_direction, FragNormal) * FragNormal) ;
    vec3 reflectionTexture = texture(cubemap, reflection).rgb ;

    float bidirectionReflection = max(dot(-light_direction, FragNormal) * light_intensity , 0.0) + bias ;
    FragColor = vec4(alpha * bidirectionReflection * vec3(0.1, 0.1, 0.1) + (1.0 - alpha) * reflectionTexture, 1.0) ;
}
