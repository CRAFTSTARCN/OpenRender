#version 430 core

in vec3 TexCoord;

uniform samplerCube Skybox;

out vec4 FragColor;

void main() {
    vec3 Color = texture(Skybox, TexCoord).rgb;
    FragColor = vec4(Color, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
}