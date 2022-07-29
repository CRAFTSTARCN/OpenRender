#pragma once
#include "MaterialParam.h"
#include "../RHI/GLHandleDef.h"

class Material {

    GLShaderProgramHandle Shader;
    MaterialParam Param;
    
    int TotalLightWeight;
    bool UseIBL;
    bool DoubleSided;
    
public:

    Material(GLShaderProgramHandle InShader, int InLightWeight = 1, bool InUseIBL = true, bool InDoubleSided = false);
    ~Material();

    void SetDefaultParam(const MaterialParam& InParam);
    void SetDefaultParam(MaterialParam&& InParam);

    MaterialParam GetDefaultParamCopy() const;
    
    void SetShaderHanle(GLShaderProgramHandle Program);
    GLShaderProgramHandle GetShaderHandle() const;

    bool IsUseIBL() const;
};
