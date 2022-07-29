#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 Tangent;
layout(location = 3) in vec2 UV;

out Vert2Frag {
    vec3 WorldPosition;
    vec3 WorldNormal;
    vec3 WorldTangent;
    vec2 UV;
} V2F;

layout (std140) uniform _TransfromMatrices
{
    mat4 _MatrixMVP;
    mat4 _MatrixM2W;
    mat4 _MatrixN2W;
};


void main() {
    gl_Position = _MatrixMVP * vec4(Position, 1.0);
    V2F.WorldPosition = (_MatrixM2W * vec4(Position, 1.0)).xyz;
    V2F.WorldNormal = (_MatrixN2W * vec4(Normal, 0.0)).xyz;
    V2F.WorldTangent = (_MatrixM2W * vec4(Tangent, 0.0)).xyz;
    V2F.UV = UV;
}