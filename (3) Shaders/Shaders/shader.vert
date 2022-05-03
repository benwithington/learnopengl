#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 ourColour;
out vec3 ourPos;

uniform float offsetPos;

void main()
{
    ourPos = vec3((aPos.x + offsetPos), (aPos.y * -1), aPos.z);
    ourColour = aColour;
    gl_Position = vec4(ourPos, 1.0);
}