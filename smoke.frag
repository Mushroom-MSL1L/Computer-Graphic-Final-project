#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D cloudTexture;
uniform float alpha;
uniform float blurStrength;
uniform int shake;
uniform vec3 light_pos;
uniform vec3 redColor;
uniform vec3 yellowColor;
uniform vec3 grayColor;
uniform float objectSize;

void main() {
    
    vec3 baseColor;
    if (objectSize <= 0.55) {
        // 小尺寸：紅色 (完全紅)
        baseColor = redColor;
    } else if (objectSize <= 2.1) {
        // 中尺寸：紅到黃過渡
        float factor = (objectSize - 0.55) / (2.1 - 0.55);
        baseColor = mix(redColor, yellowColor, factor);
    } else if (objectSize <= 2.5) {
        // 大尺寸：黃到灰過渡
        float factor = (objectSize - 2.1) / (2.5 - 2.1);
        baseColor = mix(yellowColor, grayColor, factor);
    } else {
        // 超大尺寸：完全灰
        baseColor = grayColor;
    }

   
    vec3 lightDir = normalize(light_pos - FragPos);
    vec3 norm = normalize(Normal);
    float intensity = max(dot(norm, lightDir), 0.0);

    if (intensity > 0.8) intensity = 1.0;
    else if (intensity > 0.5) intensity = 0.7;
    else if (intensity > 0.2) intensity = 0.4;
    else intensity = 0.1;


    vec3 finalColor = baseColor  * intensity;

    vec3 blurColor = finalColor;
    if (shake != 0) {
        // 使用法線來模擬模糊強度
        float blurFactor = length(Normal); // 法線長度決定模糊程度
        blurColor = mix(finalColor, finalColor * 0.5, blurFactor * blurStrength);
    }

    FragColor = vec4(blurColor, alpha);
}
