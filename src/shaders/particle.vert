#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 originalColor ;

uniform vec4 tint ;
void main() {
    gl_Position = vec4(aPos, 1.0);
    //originalColor = tint ;            // not work temporarily use below one 
    originalColor = vec4 (1.0, 0.2, 0.0, 1.0) ;
}
