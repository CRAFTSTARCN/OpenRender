#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Renderable.h"


struct Transform {
    glm::vec3 Translate;
    glm::quat Orientation;
    glm::vec3 Scale;

    static Transform Identity;

    Transform();

    Transform(const glm::vec3& InTranslate, const glm::quat& InOrientation, const glm::vec3& InScale);

    Transform(const Transform& Copy);

    Transform& operator=(const Transform& Copy);
};


class RenderableInstance {

    const Renderable *BaseRenderable;

    std::vector<RuntimeMaterial*> MaterialOverride;

    Transform InstanceTransform;
    
public:

    RenderableInstance(const Renderable *InBaseRenerable);
    ~RenderableInstance();

    void SetMaterial(int IndexSlot, RuntimeMaterial* Material);

    int GetSubMeshCount() const;
    GLVertexAssembleHandle GetSubMesh(int SlotIndex) const;
    GLIndexBufferHandle GetIndexBuffer(int SlotIndex) const;
    GLVertexBufferHandle GetVertexBuffer() const;
    const RuntimeMaterial* GetMaterial(int SlotIndex) const;
    bool IsUseIndex() const;
    int GetElementCount(int IndexSlot) const;

    const Transform* GetTransform() const;

    void SetTranslate(const glm::vec3& InTranslate);

    void SetOrientation(const glm::quat& InOrientation);

    void SetOrientation(const glm::vec3& Eular);

    void SetScale(const glm::vec3& InScale);

};
