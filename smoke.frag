#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D cloudTexture;
uniform float alpha;
uniform float u_time;
uniform float blurStrength;
uniform int shake;

void main() {
    vec4 texColor;

    if (shake == 0) {
        texColor = texture(cloudTexture, TexCoord);
        FragColor = vec4(texColor.rgb, texColor.a * alpha);
    } else {
        // 基於時間的晃動偏移
        float offset = sin(u_time * 50.0) * 0.005;
        vec2 shakenTexCoords = clamp(TexCoord + vec2(offset, -offset), vec2(0.0), vec2(1.0));


        // 增加多方向模糊偏移
        /*vec2 offsets[9] = vec2[](
            vec2(0.0, 0.0),
            vec2(blurStrength, 0.0),
            vec2(-blurStrength, 0.0),
            vec2(0.0, blurStrength),
            vec2(0.0, -blurStrength),
            vec2(blurStrength, blurStrength),
            vec2(-blurStrength, blurStrength),
            vec2(blurStrength, -blurStrength),
            vec2(-blurStrength, -blurStrength)
        );*/

        // 累加所有偏移的顏色值
        //vec4 colorSum = vec4(0.0);
        /*for (int i = 0; i < 9; ++i) {
            vec2 clampedCoords = clamp(shakenTexCoords + offsets[i], 0.0, 1.0);
            texColor = texture(cloudTexture, clampedCoords);
            colorSum += vec4(texColor.rgb, texColor.a * alpha);
        }*/

        // 計算平均顏色
        //FragColor = colorSum / 9.0;
        FragColor = texture(cloudTexture, shakenTexCoords);
    }

    
}
