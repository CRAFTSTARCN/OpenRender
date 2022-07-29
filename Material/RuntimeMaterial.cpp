#include "RuntimeMaterial.h"

RuntimeMaterial::RuntimeMaterial(const Material* Material) :
    BaseMaterial(Material), ParamChanged(false)
{
    if(Material) {
        Parameter = Material->GetDefaultParamCopy();
    }
}

RuntimeMaterial::~RuntimeMaterial() {
}

void RuntimeMaterial::SetScalarParam(const std::string& Name, float Value) {
    bool Success = Parameter.SetScalarParam(Name, Value);
    if(Success) {
        ParamChanged = 1;
    }
}

void RuntimeMaterial::SetVectorParam(const std::string& Name, const glm::vec4& Value) {
    bool Success = Parameter.SetVectorParam(Name, Value);
    if(Success) {
        ParamChanged = 1;
    }
}

void RuntimeMaterial::SetMat4Param(const std::string& Name, const glm::vec4& Value) {
    bool Success = Parameter.SetMat4Param(Name, Value);
    if(Success) {
        ParamChanged = 1;
    }
}


void RuntimeMaterial::SetTexture(const std::string& Name, Texture* Tex) {
    bool Success = Parameter.SetTexture(Name, Tex);
    if(Success) {
        ParamChanged = 1;
    }
}


const MaterialParam* RuntimeMaterial::GetParamPtr() const {
    return &Parameter;
}

const Material* RuntimeMaterial::GetMaterial() const{
    return BaseMaterial;
}

bool RuntimeMaterial::MaterialParamChanged() const {
    return ParamChanged;
}
