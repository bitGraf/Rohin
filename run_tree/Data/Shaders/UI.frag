#version 330 core
out vec4 FragColor;

in vec2 pass_uv;

uniform sampler2D tex;
uniform float factor;

void main() {
    vec4 texColor = texture(tex, pass_uv);


    float alpha = texColor.a * factor * 0.5;
    vec3 color = texColor.rgb;

    /*
    mapped = colAndFog / (colAndFog + vec3(1.0));
    
    mapped = pow(mapped, vec3(1.0/gamma));
    */
    //color = color / (color + vec3(1.0));
    const float gamma = 2.2;
    color = pow(color, vec3(1/gamma));

    FragColor = vec4(color, alpha);
}