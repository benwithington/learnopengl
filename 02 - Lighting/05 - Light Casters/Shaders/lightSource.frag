#version 330 core
out vec4 FragColour;

uniform vec3 lightSourceColour;

void main()
{
    FragColour = vec4(lightSourceColour, 1.0);
}