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
uniform float u_time;

void main() {
    
    vec4 texColor;
    if(shake == 0) {
        texColor = texture(cloudTexture, TexCoord);
    } else {
        float offset = sin(u_time * 50.0) * 0.005;
        vec2 shakenTexCoords = TexCoord + vec2(offset, -offset);

        // 定義周圍方向偏移量
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

        vec4 colorSum = vec4(0.0);
        for (int i = 0; i < 1; ++i) {
            colorSum += texture(cloudTexture, shakenTexCoords + offsets[i]);
        }

        // 計算平均值
        texColor = colorSum;
    }
    
    vec3 baseColor;
    if (objectSize <= 0.35) {
        // 中尺寸：紅到黃過渡
        float factor = (objectSize - 0.35) / 0.35;
        baseColor = mix(texColor.rgb, grayColor, factor);
    } 
    else {
        // 超大尺寸：完全灰
        baseColor = grayColor * texColor.rgb;
    }

   
    vec3 lightDir = normalize(light_pos - FragPos);
    vec3 norm = normalize(Normal);
    float intensity = max(dot(norm, lightDir), 0.0);

    if (intensity > 0.8) intensity = 1.0;
    else if (intensity > 0.5) intensity = 0.7;
    else if (intensity > 0.2) intensity = 0.4;
    else intensity = 0.1;


    vec3 finalColor = baseColor * intensity;
    //vec4 finalColor = texture(cloudTexture,TexCoord);
    FragColor = vec4(finalColor.rgb, alpha);
}
