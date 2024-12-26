# version 330 core

layout ( points ) in;
layout ( triangle_strip, max_vertices = 24 ) out; 
// 24 = 4 particle results * 6 faces 

in vec4 originalColor[];

out vec3 finalColor ; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float particleSize ;

const vec3 lightDirection = normalize(vec3(10.0, 100.0, -100.0)) ;

mat4 transformation = projection * view * model ;

void createVertex(vec3 offset, vec3 normal) {
    vec4 actualOffset = vec4(offset * particleSize, 0.0) ;
    vec4 worldPosition = gl_in[0].gl_Position + actualOffset ; 
    gl_Position = transformation * worldPosition ;

    float brightness = max(dot(-lightDirection, normal), 0.4) ;
    finalColor = originalColor[0].rgb * brightness ;

    EmitVertex() ;
}

void createFace(vec3 a, vec3 b, vec3 c, vec3 d) {
    vec3 v1 = b - a ;
    vec3 v2 = c - a ;
    vec3 normal = cross(normalize(v1), normalize(v2)) ;
    createVertex(a, normal) ;
    createVertex(b, normal) ;
    createVertex(c, normal) ;
    createVertex(d, normal) ;
}

void main(void) {
    vec3 offsets[8] = vec3[](
        vec3(-1.0, -1.0, -1.0), // 0: 左下後
        vec3( 1.0, -1.0, -1.0), // 1: 右下後
        vec3(-1.0,  1.0, -1.0), // 2: 左上後
        vec3( 1.0,  1.0, -1.0), // 3: 右上後
        vec3(-1.0, -1.0,  1.0), // 4: 左下前
        vec3( 1.0, -1.0,  1.0), // 5: 右下前
        vec3(-1.0,  1.0,  1.0), // 6: 左上前
        vec3( 1.0,  1.0,  1.0)  // 7: 右上前
    );

    createFace(offsets[0], offsets[1], offsets[2], offsets[3]); // 後面
    createFace(offsets[4], offsets[5], offsets[6], offsets[7]); // 前面
    createFace(offsets[0], offsets[4], offsets[2], offsets[6]); // 左面
    createFace(offsets[1], offsets[5], offsets[3], offsets[7]); // 右面
    createFace(offsets[2], offsets[3], offsets[6], offsets[7]); // 上面
    createFace(offsets[0], offsets[1], offsets[4], offsets[5]); // 下面
}