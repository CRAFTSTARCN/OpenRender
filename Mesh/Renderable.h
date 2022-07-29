#pragma once
#include <vector>

#include "../Material/RuntimeMaterial.h"
#include "../RHI/GLHandleDef.h"

class Renderable {

    GLVertexBufferHandle VertexBuffer;
    std::vector<GLIndexBufferHandle> IndexBuffers;
    std::vector<GLVertexAssembleHandle> SubMeshes;
    std::vector<int> ElementCount;


    std::vector<RuntimeMaterial*> Materials;

    bool UseIndex;
    int SubMeshCount;
    
public:

    Renderable(GLVertexBufferHandle InVertexBuffer, std::vector<GLIndexBufferHandle>&& InIndexBuffers, std::vector<GLVertexAssembleHandle>&& InSubMeshes, std::vector<int> InElementCount);
    ~Renderable();

    void SetMaterial(int IndexSlot, RuntimeMaterial* Material);

    int GetSubMeshCount() const;
    GLVertexAssembleHandle GetSubMesh(int SlotIndex) const;
    GLIndexBufferHandle GetIndexBuffer(int SlotIndex) const;
    GLVertexBufferHandle GetVertexBuffer() const;
    const RuntimeMaterial* GetMaterial(int SlotIndex) const;

    int GetElementCount(int IndexSlot) const;
    bool IsUseIndex() const;
    
};
