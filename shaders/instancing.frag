#version 330 core

out vec4 fragColour;

in vec3 fColour;

void main() { fragColour = vec4(fColour, 1.0); }