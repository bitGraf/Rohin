#version 330 core
out vec4 FragColor;

in vec3 pass_fragPos;
//in vec3 pass_color;

uniform vec3 camPos;
uniform vec4 color;

float clampRamp(float val, float xmin, float xmax, float ymin, float ymax);

void main()
{
    float distance = length(pass_fragPos - camPos);
    float strength = clampRamp(distance, 1, 3, .4, 1);

    FragColor = color * strength;

    //FragColor = vec4(vec3(strength), 1);
}

float clampRamp(float val, float xmin, float xmax, float ymin, float ymax) {
    return min(max(
        (ymin-ymax)/(xmax-xmin)*val + (ymax - xmin*(ymin-1)/(xmax-xmin))
        ,ymin), ymax);
}