#version 330 core
out vec4 FragColour;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 normal;
in vec3 FragPos;

void main()
{
    vec3 norm = normalize(normal);


    //Ambient Lighting
    float ambientStrength = 0.25f;
    vec3 ambient = ambientStrength * lightColour;

    //Diffuse Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColour;

    //Specular Lighting
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;

    FragColour = vec4((ambient + diffuse + specular) * objectColour, 1.0);
}