#version 330 core
out vec4 FragColour;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

in vec3 normal;
in vec3 FragPos;

void main()
{
    vec3 norm = normalize(normal);

    //Ambient Lighting
    vec3 ambient = light.ambient * material.ambient;

    //Diffuse Lighting
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    //Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

    FragColour = vec4((ambient + diffuse + specular), 1.0);
}