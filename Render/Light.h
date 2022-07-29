#pragma once
#include "../RHI/UniformType.h"

enum LightType {
    Directional,
    PointLight,
    SpotLight
};

class Light {

protected:
    UniformBlock LightDataBlock;
    glm::vec4 LightColor;
    LightType Type;
    
public:

    Light(LightType InType);
    virtual ~Light();

    const UniformBlock* GetUniformBlock();
    void SetColor(glm::vec4 Color);
};

class DirectionalLight : public Light {

protected:

    glm::vec4 Direction;
    
public:
    DirectionalLight();

    void SetDirection(const glm::vec4 InDirection);
    
    ~DirectionalLight();
    
};
