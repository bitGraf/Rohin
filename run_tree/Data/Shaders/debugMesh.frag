#version 330 core
layout (location = 0) out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;

struct DirectionalLight {
	vec3 direction;
	vec4 color;
	float strength;
};

uniform DirectionalLight sun;
uniform vec3 objColor;
uniform vec3 camPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * sun.color.rgb;
  	
    // diffuse 
    vec3 norm = normalize(pass_normal);
    vec3 lightDir = normalize(-sun.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * sun.color.rgb;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(camPos - pass_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * sun.color.rgb;  
        
    vec3 result = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(result, 1.0);
}