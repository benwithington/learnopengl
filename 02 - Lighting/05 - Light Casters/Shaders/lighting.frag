#version 330 core
out vec4 FragColour;

struct Material {
    sampler2D   diffuse;
    sampler2D   specular;
    sampler2D   emission;
    float       shininess;
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
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(normal);

    //Ambient Lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //Diffuse Lighting
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    //Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    //Emission Lighting
    vec3 emission = vec3(texture(material.emission, TexCoords));

    FragColour = vec4((ambient + diffuse + specular + emission), 1.0);
}