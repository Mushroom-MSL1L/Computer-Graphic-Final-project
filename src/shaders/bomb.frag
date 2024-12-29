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

uniform float blurStrength;
uniform int shake;

vec3 calculateLighting(vec3 fragPosition, vec3 fragNormal, vec4 textureColor) {
    vec3 lightDir = normalize(light_position - fragPosition);
    vec3 viewDir = normalize(camera_position - fragPosition);

    vec3 ambient = light_ambient;

    vec3 diffuse = light_diffuse * max(dot(lightDir, fragNormal), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 specular = light_specular * pow(max(dot(fragNormal, halfwayDir), 0.0), material_gloss);

    return (ambient + diffuse + specular) * textureColor.rgb;
}


void main()
{
    vec4 textureColor = texture(ourTexture, TexCoord);
    vec4 colorSum = vec4(0.0);

    vec2 texCoords = TexCoord;
    if (shake != 0) {
        float offset = sin(time * 50.0) * 0.005;
        texCoords += vec2(offset, -offset);

        vec2 offsets[9] = vec2[](
            vec2(0.0, 0.0),
            vec2(blurStrength, 0.0),
            vec2(-blurStrength, 0.0),
            vec2(0.0, blurStrength),
            vec2(0.0, -blurStrength),
            vec2(blurStrength, blurStrength),
            vec2(-blurStrength, blurStrength),
            vec2(blurStrength, -blurStrength),
            vec2(-blurStrength, -blurStrength)
        );

        for (int i = 0; i < 9; ++i) {
            colorSum += texture(ourTexture, texCoords + offsets[i]);
        }

        FragColor = colorSum / 9.0;
        return;
    }

    if (isEffect == 1.0) {
        FragColor = vec4(effectColor, 1.0);
        return;
    }

    if (isEffect > 0.0) {
        vec3 lighting = calculateLighting(FragPosition, FragNormal, vec4(effectColor, 1.0));
        FragColor = vec4(lighting, 1.0);
        return;
    }

    if (isSpark > 0.0) {
        float glow = isSpark * 2.0;
        FragColor = vec4(sparkColor * glow, 1.0);
        return;
    }

    vec3 lighting = calculateLighting(FragPosition, FragNormal, textureColor);
    FragColor = vec4(lighting, textureColor.a);
}
