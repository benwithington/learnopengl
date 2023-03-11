#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;

in VS_OUT { vec3 colour; }
gs_in[];

out vec3 fColour;

void build_house(vec4 position) {
    fColour = gs_in[0].colour;

    // bottom left
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    // bottom right
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    // top left
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    // top right
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    // roof
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
    fColour = vec3(1.0, 1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() { build_house(gl_in[0].gl_Position); }