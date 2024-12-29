#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 rainbowColor;

uniform float u_time;
uniform float blurStrength;
uniform int shake;

void main() {
    // 計算時間驅動的晃動偏移
    if(shake == 0) {
        FragColor = texture(ourTexture, TexCoord);
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
        for (int i = 0; i < 9; ++i) {
            colorSum += texture(ourTexture, shakenTexCoords + offsets[i]);
        }

        // 計算平均值
        FragColor = colorSum / 9.0;
    }
}
