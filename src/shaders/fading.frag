#version 330 core
out vec4 FragColor;

uniform float alpha;  // Controls the transparency of the white overlay

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, alpha);  // White color with varying alpha
}
