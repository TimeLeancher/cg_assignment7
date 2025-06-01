#version 330 core
in vec3 FragPos;
in vec3 Normal;
out vec4 outColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

void main() {
    // Ambient
    vec3 ambient = 0.2 * ka;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float differ = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = kd * differ;

    // Specular 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = ks * spec * differ; 

    vec3 result = (ambient + diffuse + specular) * lightColor;
    result = pow(result, vec3(1.0 / 2.2));
    outColor = vec4(result, 1.0);
}