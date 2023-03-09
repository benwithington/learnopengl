#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform bool shouldReflect;

vec4 reflection(vec3 view) {
    vec3 R = reflect(view, normalize(Normal));
    return vec4(texture(skybox, R).rgb, 1.0);
}

vec4 refraction(vec3 view, float ratio) {
    vec3 R = refract(view, normalize(Normal), ratio);
    return vec4(texture(skybox, R).rgb, 1.0);
}

void main() {
    vec3 view = normalize(Position - cameraPos);

    if (shouldReflect) {
        FragColor = reflection(view);
    } else {
        FragColor = refraction(view, 1.0 / 1.52);
    }
}