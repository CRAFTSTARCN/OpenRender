#pragma once
#include <unordered_map>

#include "../RHI/UniformType.h"
#include "../Texture/Texture.h"

class MaterialParam {

    std::unordered_map<std::string, Uniform> UniformParams;
    std::unordered_map<std::string, Texture*> Textures;
    
public:

    using ParamTable = std::unordered_map<std::string, Uniform>;
    using TextureTable = std::unordered_map<std::string, Texture*>;

    MaterialParam();
    MaterialParam(const MaterialParam& Copy);
    MaterialParam(MaterialParam&& Move) noexcept;
    
    ~MaterialParam();
    
    bool AddParam(const std::string& Name, UniformType Type);
    bool AddTextureParam(const std::string& Name, Texture* Tex);

    bool SetScalarParam(const std::string& Name, float Value);
    bool SetVectorParam(const std::string& Name, const glm::vec4& Value);
    bool SetMat4Param(const std::string& Name, const glm::vec4& Value);
    bool SetTexture(const std::string& Name, Texture* Tex);

    MaterialParam& operator=(const MaterialParam& Copy);
    MaterialParam& operator=(MaterialParam&& Move) noexcept;

    const ParamTable* GetParamTable() const;
    const TextureTable* GetTextureTable() const;
    
};
