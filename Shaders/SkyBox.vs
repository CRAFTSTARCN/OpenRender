#version 430 core

layout(location = 0) vec3 Position;

uniform mat4 _MVP;

out vec3 TexCoord;

void main() {
    vec3 P = -_MVP * vec4(Position, 1.0f);
    gl_Position = P.xyww;

    TexCoord = Position;
}