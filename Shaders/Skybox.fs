#version 430 core

in vec3 TexCoord;

uniform samplerCube Skybox

out vec4 FragColor;

void main() {
    FragColor = texture(Skybox, TexCoord);
}