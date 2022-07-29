#include "Material.h"

Material::Material(GLuint InShader, int InLightWeight, bool InUseIBL,  bool InDoubleSided) :
    Shader(InShader), TotalLightWeight(InLightWeight), UseIBL(InUseIBL), DoubleSided(InDoubleSided)
{
}

Material::~Material() {
}

void Material::SetDefaultParam(const MaterialParam& InParam) {
    Param = InParam;
}

void Material::SetDefaultParam(MaterialParam&& InParam) {
    Param = std::move(InParam);
}

MaterialParam Material::GetDefaultParamCopy() const {
    MaterialParam Copy = Param;
    return Copy;
}

void Material::SetShaderHanle(GLuint Program) {
    Shader = Program;
}

GLuint Material::GetShaderHandle() const {
    return Shader;
}

bool Material::IsUseIBL() const {
    return UseIBL;
}

