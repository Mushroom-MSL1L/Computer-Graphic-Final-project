#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 256) out;

out vec2 TexCoord;
out vec3 FragNormal;
out vec3 FragPosition;
out float isSpark;
out vec3 sparkColor;
out float isEffect;
out vec3 effectColor;

in VS_OUT {
    vec2 TexCoord;
    vec3 FragNormal;
    vec3 FragPosition;
    vec4 worldPosition;
} gs_in[];

uniform float time;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform float sparkStartTime;
uniform float sparkDuration; 
uniform float crackStartTime;
uniform float crackDuration;
uniform float detachStartTime;
uniform float detachDuration;

const float maxDistance = 100.0;
const float sparkSpeed = 2.0;
const int maxSparks = 10;
const float sparkSize = 0.05;
const float crackScale = 20.0;
const vec3 startSparkColor = vec3(0.9, 0.3, 0.1);
const vec3 endSparkColor = vec3(1.0, 0.6, 0.2); 

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec4 getTriangleCenter() {
    return (gs_in[0].worldPosition +
            gs_in[1].worldPosition +
            gs_in[2].worldPosition) / 3.0;
}

vec3 getNormal() {
    vec3 a = vec3(gs_in[1].worldPosition) - vec3(gs_in[0].worldPosition);
    vec3 b = vec3(gs_in[2].worldPosition) - vec3(gs_in[0].worldPosition);
    return normalize(cross(a, b));
}

void emitSpark(vec4 position, vec3 normal, vec3 color, float alive) {
    if (alive <= 0.0) return;

    vec4 p1 = position + vec4(-sparkSize, -sparkSize, 0.0, 0.0);
    vec4 p2 = position + vec4(sparkSize, -sparkSize, 0.0, 0.0);
    vec4 p3 = position + vec4(0.0, sparkSize, 0.0, 0.0);

    gl_Position = projection * view * p1;
    TexCoord = vec2(0.0, 0.0);
    FragNormal = normalize(normal);
    FragPosition = vec3(p1);
    isSpark = alive;
    isEffect = 0.0;
    sparkColor = color;
    EmitVertex();

    gl_Position = projection * view * p2;
    TexCoord = vec2(0.0, 0.0);
    FragNormal = normalize(normal);
    FragPosition = vec3(p2);
    isSpark = alive;
    isEffect = 0.0;
    sparkColor = color;
    EmitVertex();

    gl_Position = projection * view * p3;
    TexCoord = vec2(0.0, 0.0);
    FragNormal = normalize(normal);
    FragPosition = vec3(p3);
    isSpark = alive;
    isEffect = 0.0;
    sparkColor = color;
    EmitVertex();

    EndPrimitive();
}

void Spark() {
    if (time < sparkStartTime || time > sparkStartTime + sparkDuration) {
        return;
    }

    vec4 center = getTriangleCenter();
    vec3 normal = getNormal();
    float elapsedTime = time - sparkStartTime;
    float normalizedTime = elapsedTime / sparkDuration;

    float chance = random(center.xy + time);
    if(chance > 0.1){
        return;
    }

    int localMaxSparks = int(maxSparks * (0.5 + random(vec2(gl_PrimitiveID, time)) * 0.5));
    for (int i = 0; i < localMaxSparks; i++) {
        float distance = sparkSpeed * elapsedTime + random(vec2(gl_PrimitiveID, time+i)) * 2.0;
        if (distance > maxDistance) continue;

        float alive = exp(-distance / (maxDistance * 0.8));

        float randomOffset = random(center.xy + float(gl_PrimitiveID) + time);
        vec3 offset = normalize(normal * 0.7 + randomOffset * 0.3);

        vec3 sparkColor = mix(startSparkColor, endSparkColor, pow(normalizedTime, 1.2));
        sparkColor += vec3(0.05, 0.02, 0.0) * alive;
        vec4 sparkPos = center + vec4((normal + offset) * distance, 0.0);
        emitSpark(sparkPos, normal, sparkColor, alive);
    }
}

vec4 explode(vec4 position, vec3 normal, float magnitude){
    vec3 direction = normal* magnitude;
    return position + vec4(direction, 0.0);
}

void Crack() {
    if (time < crackStartTime || time > crackStartTime + crackDuration) {
        return;
    }

    vec4 p1 = explode(gs_in[0].worldPosition, gs_in[0].FragNormal, 1.1);
    vec4 p2 = explode(gs_in[1].worldPosition, gs_in[1].FragNormal, 1.1);
    vec4 p3 = explode(gs_in[2].worldPosition, gs_in[2].FragNormal, 1.1);

    gl_Position = projection * view * p1;
    TexCoord = gs_in[0].TexCoord;
    FragNormal = getNormal();
    FragPosition = gs_in[0].FragPosition;
    isSpark = 0.0;
    isEffect = 0.0;
    EmitVertex();

    gl_Position = projection * view * p2;
    TexCoord = gs_in[1].TexCoord;
    FragNormal = getNormal();
    FragPosition = gs_in[1].FragPosition;
    isSpark = 0.0;
    isEffect = 0.0;
    EmitVertex();

    gl_Position = projection * view * p3;
    TexCoord = gs_in[2].TexCoord;
    FragNormal = getNormal();
    FragPosition = gs_in[2].FragPosition;
    isSpark = 0.0;
    isEffect = 0.0;
    EmitVertex();

    EndPrimitive();
}

void Detach(){
    if (time < detachStartTime || time > detachStartTime + detachDuration) {
        return;
    }

    vec3 normal = getNormal();

    float elapsedTime = time - detachStartTime;
    float normalizedTime = elapsedTime / detachDuration;

    float effectStrength;
    if (normalizedTime <= 0.8) {
        effectStrength = pow(clamp(normalizedTime / 0.8, 0.0, 1.0), 2.0);
    } else {
        effectStrength = 1.0;
    }
    float spread = normalizedTime * 2.0;
    vec3 startEffectColor = vec3(0.1, 0.1, 0.1);
    vec3 endEffectColor = vec3(1.0, 1.0, 1.0);
    vec3 dynamicEffectColor = mix(startEffectColor, endEffectColor, effectStrength);
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        TexCoord = gs_in[i].TexCoord;
        FragNormal = gs_in[i].FragNormal;
        FragPosition = gs_in[i].FragPosition;
        isSpark = 0.0;
        isEffect = 1.0;
        effectColor = clamp(dynamicEffectColor * effectStrength * spread, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();

    //Detach Part
    vec3 startDetachColor = vec3(0.1, 0.1, 0.1);
    vec3 endDetachColor = vec3(1.0, 1.0, 1.0);
    float t = normalizedTime;
    float speedFactor = exp(pow(t, 2.0) * 4.0);
    vec3 effectColorTransition = vec3(
        sin(6.28318 * (t + 0.0) * speedFactor),
        sin(6.28318 * (t + 0.33) * speedFactor),
        sin(6.28318 * (t + 0.66) * speedFactor)
    ) * 0.5 + 0.5;
    for (int i = 0; i < 3; i++) {
        gl_Position = projection * view * explode(gs_in[i].worldPosition, normal, 1.1);
        TexCoord = gs_in[i].TexCoord;
        FragNormal = gs_in[i].FragNormal;
        FragPosition = gs_in[i].FragPosition;
        isSpark = 0.0;
        isEffect = 0.5;
        effectColor = clamp(effectColorTransition, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}

void LensFlare() {
    if (time < sparkStartTime || time > sparkStartTime + sparkDuration) {
        return;
    }
    vec4 position = getTriangleCenter();
    const float flareSize = 1.0;

    for (int i = 0; i < 3; i++) {
        vec4 flarePos = position + vec4(
            cos(float(i) * 3.141592 / 2.0) * flareSize,
            sin(float(i) * 3.141592 / 2.0) * flareSize,
            0.0, 0.0
        );

        gl_Position = projection * view * flarePos;
        TexCoord = vec2(0.5, 0.5);
        FragNormal = vec3(0.0, 0.0, 1.0);
        FragPosition = vec3(flarePos);
        isSpark = 0.0;
        isEffect = 1.0;
        effectColor = vec3(1.0, 1.0, 0.5); // フレアの色
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        TexCoord = gs_in[i].TexCoord;
        FragNormal = gs_in[i].FragNormal;
        FragPosition = gs_in[i].FragPosition;
        isSpark = 0.0;
        isEffect = 0.0;
        EmitVertex();
    }
    EndPrimitive();
    
    Spark();
    LensFlare();
    Crack();
    Detach();
}
