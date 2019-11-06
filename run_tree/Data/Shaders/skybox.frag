#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, TexCoords);
    //FragColor = textureLod(skybox, TexCoords, 1.2);
    //FragColor = vec4(pow(texture(skybox, TexCoords).rgb, vec3(2.2)), 1);
}