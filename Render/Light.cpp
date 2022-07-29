#include "Light.h"

#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Light::Light(LightType InType) :
    LightDataBlock(4*4*sizeof(float)),
    Type(InType),
    LightColor({1.0f, 1.0f, 1.0f, 0.0f})
{
    memcpy_s(LightDataBlock.Data, 4*sizeof(float), glm::value_ptr(LightColor), 4*sizeof(float));
}

Light::~Light() {
    
}

const UniformBlock* Light::GetUniformBlock() {
    return &LightDataBlock;
}

void Light::SetColor(glm::vec4 Color) {
    LightColor = Color;
    uint8_t* DataPtr = (uint8_t*)LightDataBlock.Data;
    memcpy_s(DataPtr, 4*sizeof(float), glm::value_ptr(Color), 4*sizeof(float));
}

DirectionalLight::DirectionalLight() :
    Light(Directional), Direction{0.0,1.0,0.0,0.0}
{
    uint8_t* DataPtr = (uint8_t*)LightDataBlock.Data;
    DataPtr += 4*sizeof(float);
    memcpy_s(DataPtr, 4*sizeof(float), glm::value_ptr(Direction), 4*sizeof(float));
}

void DirectionalLight::SetDirection(const glm::vec4 InDirection) {
    Direction = glm::normalize(InDirection);
    uint8_t* DataPtr = (uint8_t*)LightDataBlock.Data;
    DataPtr += 4*sizeof(float);
    memcpy_s(DataPtr, 4*sizeof(float), glm::value_ptr(Direction), 4*sizeof(float));
}

DirectionalLight::~DirectionalLight() {
}
