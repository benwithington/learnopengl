#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform bool showDepth;
uniform sampler2D texture1;

float near = 0.1;
float far = 100.0;

float lineariseDepth(float depth);

void main() {
    if (showDepth) {
        // float depth = lineariseDepth(gl_FragCoord.z) / far;
        float depth = gl_FragCoord.z;
        FragColor = vec4(vec3(depth), 1.0);
    } else {
        FragColor = texture(texture1, TexCoords);
    }
}

float lineariseDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to ndc
    return (2.0 * near * far) / (far + near - z * (far - near));
}