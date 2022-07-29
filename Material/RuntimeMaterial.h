#pragma once
#include "Material.h"

class RuntimeMaterial {

    const Material *BaseMaterial;
    MaterialParam Parameter;
    bool ParamChanged;
    
public:

    RuntimeMaterial(const Material* Material);
    ~RuntimeMaterial();

    void SetScalarParam(const std::string& Name, float Value);
    void SetVectorParam(const std::string& Name, const glm::vec4& Value);
    void SetMat4Param(const std::string& Name, const glm::vec4& Value);
    void SetTexture(const std::string& Name, Texture* Tex);

    const MaterialParam* GetParamPtr() const;
    const Material* GetMaterial() const;
    bool MaterialParamChanged() const;
};
