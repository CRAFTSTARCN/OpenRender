#include "Renderable.h"

Renderable::Renderable(GLuint InVertexBuffer, std::vector<GLuint>&& InIndexBuffers, std::vector<GLuint>&& InSubMeshes, std::vector<int> InElementCount) :
    VertexBuffer(InVertexBuffer), IndexBuffers(std::move(InIndexBuffers)), SubMeshes(std::move(InSubMeshes)), ElementCount(std::move(InElementCount))
{
    UseIndex = IndexBuffers.size() != 0;
    SubMeshCount = (int)SubMeshes.size();
    Materials.resize(SubMeshCount, nullptr);
}

Renderable::~Renderable() {
    for(auto Material : Materials) {
        if(Material) {
            delete Material;
        }
    }
}

void Renderable::SetMaterial(int IndexSlot, RuntimeMaterial* Material) {
    if(IndexSlot >= 0 && IndexSlot < SubMeshCount && Material != nullptr) {
        if(Materials[IndexSlot] != nullptr) {
            delete Materials[IndexSlot];
        }
        Materials[IndexSlot] = Material;
    }
}

int Renderable::GetSubMeshCount() const {
    return SubMeshCount;
}

GLuint Renderable::GetSubMesh(int SlotIndex) const {
    if(SlotIndex >= 0 && SlotIndex < SubMeshCount) {
        return SubMeshes[SlotIndex];
    }

    return 0;
}

GLuint Renderable::GetIndexBuffer(int SlotIndex) const {
    if(SlotIndex >= 0 && SlotIndex < (int)IndexBuffers.size()) {
        return IndexBuffers[SlotIndex];
    }

    return 0;
}

GLuint Renderable::GetVertexBuffer() const {
    return VertexBuffer;
}

const RuntimeMaterial* Renderable::GetMaterial(int SlotIndex) const {
    if(SlotIndex >= 0 && SlotIndex < SubMeshCount) {
        return Materials[SlotIndex];
    }
    
    return nullptr;
}

int Renderable::GetElementCount(int IndexSlot) const {
    if(IndexSlot >= 0 && IndexSlot < SubMeshCount) {
        return ElementCount[IndexSlot];
    }
    return 0;
}

bool Renderable::IsUseIndex() const {
    return UseIndex;
}
