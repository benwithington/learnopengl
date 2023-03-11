#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec2 offset;

out vec3 fColour;

void main() {
    vec2 temp = pos * (gl_InstanceID / 100.0);
    gl_Position = vec4(temp + offset, 0.0, 1.0);
    fColour = colour;
}