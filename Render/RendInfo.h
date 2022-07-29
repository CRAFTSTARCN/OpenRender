#pragma once
#include "../Material/Material.h"
#include "../Mesh/RenderableInstance.h"
#include "../RHI/GLHandleDef.h"
#include "../RHI/OpenGLRHI.h"

struct RendInfo {
    const Material* UseMaterial;
    const MaterialParam* Params;
    const Transform* ObjectTransform;
    DrawMod ObjectDrawMode;

    int Count;
    int Offset;
    float ViewCoordZ;
    GLVertexAssembleHandle VertexAssemble;

    bool UseElement;
    bool ParamChanged;
};
