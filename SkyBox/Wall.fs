#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube wall;

void main()
{    
    FragColor = texture(wall, TexCoords);
}