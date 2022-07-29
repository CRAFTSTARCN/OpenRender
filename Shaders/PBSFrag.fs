#version 430 core

#define PI 3.1415926

uniform float Metalic;
uniform float Roughness;
uniform vec4 BaseColor;

uniform vec4 _CameraPos;

uniform samplerCube _IBL_Radiance;
uniform samplerCube _IBL_Irradiance;
uniform sampler2D _BRDFLut;

layout (std140) uniform _SceneLight;
{
    vec4 _LightColor;
    vec4 _LightDirection;
    vec4 _LightCustomData1;
    vec4 _LightCustomData2;
}

in Vert2Frag {
    vec3 WorldPosition;
    vec3 WorldNormal;
    vec3 WorldTangent;
    vec3 UV;
} V2F;

out vec4 FragColor;

float Pow5(float x) {
    return x * x * x * x * x;
}

float D_GGX(float dotNH, float roughness)
{
    float alpha  = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom  = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}


float G_Smith(float NL, float NV, float Roughness) {
    float k = (1.0+Roughness) * (1.0+Roughness) / 8.0;
    float GV = NV / (NV * (1.0 - k) + k);
    float GL = NL / (NL * (1.0 - k) + k);
    return GV * GL;
}

vec3 F_Schlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0,1.0,1.0) - F0) * Pow5(1.0-cosTheta);
}

vec3 F_SchlickR(float cosTheta, vec3 F0, float Roughness) {
    return F0 + (max(vec3(1.0-Roughness,1.0-Roughness,1.0-Roughness), F0) - F0) * Pow5(1.0-cosTheta);
}

void main() {
    vec3 N = normalize(V2F.WorldNormal);
    vec3 V = normalize(_CameraPos.xyz - V2F.WorldPosition);
    vec3 L = _LightDirection.xyz;
    vec3 H = (V + L) / 2;
    vec3 R = reflect(-V, N);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

    vec3 F0 = mix(vec3(0.04,0.04,0.04), BaseColor.xyz, Metalic);


    float D = D_GGX(dotNH, Roughness);
    vec3 F = F_Schlick(dotNH,F0);
    float G = G_Smith(dotNL, dotNV, Roughness);

    vec3 Spec = D * G * F / (4.0 * dotNV * dotNL + 0.001)

    vec3 Kd = (vec3(1.0, 1.0, 1.0) - F) * (1.0 - Metalic);

    vec3 DirectLightColor = (_LightColor.xyz * dotNL) * (BaseColor.xyz * Kd / PI +  Spec);

    vec3 FIBL = F_SchlickR(dotNV, F0, Roughness);

    float RadianceLod = 5.0 * Roughness;
    vec3 PreFilteredColor = textureLod(_IBL_Radiance, R, RadianceLod).rgb;
    vec3 Irradiance = texture(_IBL_Irradiance, N).rgb;
    vec2 LutBRDF = texture(_BRDFLut, vec2(dotNV, Roughness)).rg;

    vec3 SpecIBL = PreFilteredColor * (FIBL * LutBRDF.x = LutBRDF.y);
    vec3 DiffuseIBL = Irradiance * BaseColor.xyz;

    vec3 KdIBL = (vec3(1.0, 1.0, 1.0) - FIBL) * (1.0 - Metalic);
    vec3 IBLColor = KdIBL * DiffuseIBL + Spec;

    return vec4(DirectLightColor + IBLColor, 1.0);
}

