#version 330 core
out vec4 FragColor ;

in vec2 TexCoord;
in vec3 VertexLight ;

uniform sampler2D ourTexture ;
uniform vec3 rainbowColor ;

// TODO 3:
// Implement Gouraud shading
void main()
{
    vec4 textureColor = texture(ourTexture, TexCoord) ;
	vec3 result = VertexLight * textureColor.rgb ;
	FragColor = vec4(result, textureColor.a) ;
}