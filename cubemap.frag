#version 330 core
out vec4 FragColor;

in vec3 TexCoords; // Cubemap 的方向向量
uniform samplerCube cube; // Cubemap 紋理
uniform float u_time;       // 時間參數
uniform float blurStrength; // 模糊強度
uniform int shake;

void main() {
    // 計算時間驅動的晃動偏移
    if(shake == 0) FragColor = texture(cube, TexCoords);

    else
    {
    float offset = sin(u_time * 50.0) * 0.005;
    vec3 shakenTexCoords = TexCoords + vec3(offset, -offset, offset);

    // 定義周圍方向偏移量
    vec3 offsets[6] = vec3[](
        vec3( blurStrength, 0.0, 0.0),
        vec3(-blurStrength, 0.0, 0.0),
        vec3(0.0,  blurStrength, 0.0),
        vec3(0.0, -blurStrength, 0.0),
        vec3(0.0, 0.0,  blurStrength),
        vec3(0.0, 0.0, -blurStrength)
    );

    // 初始化顏色
    vec4 color = texture(cube, shakenTexCoords);

    // 累加采樣的顏色
    for (int i = 0; i < 6; ++i) {
        vec3 sampleDir = normalize(shakenTexCoords + offsets[i]);
        color += texture(cube, sampleDir);
    }

    // 計算平均值
    FragColor = color / 7.0;
    }
}
