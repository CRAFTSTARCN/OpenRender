#include "MaterialParam.h"

#include <glm/ext/matrix_transform.hpp>

MaterialParam::MaterialParam() {
    
}

MaterialParam::MaterialParam(const MaterialParam& Copy) :
    UniformParams(Copy.UniformParams), Textures(Copy.Textures)
{
}

MaterialParam::MaterialParam(MaterialParam&& Move) noexcept :
    UniformParams(std::move(Move.UniformParams)), Textures(std::move(Move.Textures))
{
}

MaterialParam::~MaterialParam() {
}

bool MaterialParam::AddParam(const std::string& Name, UniformType Type) {
    auto IterParam = UniformParams.find(Name);
    auto TextureIter = Textures.find(Name);
    if(IterParam != UniformParams.end() || TextureIter != Textures.end()) {
        return false;
    }

    UniformParams.insert({Name,Uniform(Type)});
    return true;
}

bool MaterialParam::AddTextureParam(const std::string& Name, Texture* Tex) {
    auto IterParam = UniformParams.find(Name);
    auto TextureIter = Textures.find(Name);
    if(IterParam != UniformParams.end() || TextureIter != Textures.end()) {
        return false;
    }

    Textures.insert({Name, Tex});
    return true;
}

bool MaterialParam::SetScalarParam(const std::string& Name, float Value) {
    auto IterParam = UniformParams.find(Name);
    if(IterParam != UniformParams.end() && IterParam->second.Type == UniformTypeFloat) {
        memcpy_s(IterParam->second.Data, sizeof(float), &Value, sizeof(float));
        return true;
    }

    return false;
}

bool MaterialParam::SetVectorParam(const std::string& Name, const glm::vec4& Value) {
    auto IterParam = UniformParams.find(Name);
    if(IterParam != UniformParams.end() && IterParam->second.Type == UniformTypeVector4) {
        memcpy_s(IterParam->second.Data, sizeof(glm::vec4), &Value, sizeof(glm::vec4));
        return true;
    }

    return false;
}

bool MaterialParam::SetMat4Param(const std::string& Name, const glm::vec4& Value) {
    auto IterParam = UniformParams.find(Name);
    if(IterParam != UniformParams.end() && IterParam->second.Type == UniformTypeMatrix4_4) {
        memcpy_s(IterParam->second.Data, sizeof(glm::mat4), &Value, sizeof(glm::mat4));
        return true;
    }

    return false;
}

bool MaterialParam::SetTexture(const std::string& Name, Texture* Tex) {
    auto IterTex = Textures.find(Name);
    if(IterTex != Textures.end()) {
        IterTex->second = Tex;
        return true;
    }

    return false;
}

MaterialParam& MaterialParam::operator=(const MaterialParam& Copy) {
    UniformParams = Copy.UniformParams;
    Textures = Copy.Textures;
    return *this;
}

MaterialParam& MaterialParam::operator=(MaterialParam&& Move) noexcept {
    UniformParams = std::move(Move.UniformParams);
    Textures = std::move(Move.Textures);
    return *this;
}

const MaterialParam::ParamTable* MaterialParam::GetParamTable() const {
    return &UniformParams;
}

const MaterialParam::TextureTable* MaterialParam::GetTextureTable() const {
    return &Textures;
}
