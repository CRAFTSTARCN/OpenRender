#include "RenderableInstance.h"

Transform Transform::Identity = {
    {0, 0, 0},
    glm::quat(glm::radians(glm::vec3{0, 0, 0})),
    {1, 1, 1},
};


Transform::Transform() {
    *this = Identity;
}

Transform::Transform(const glm::vec3& InTranslate, const glm::quat& InOrientation, const glm::vec3& InScale) :
    Translate(InTranslate), Orientation(InOrientation), Scale(InScale)
{
}

Transform::Transform(const Transform& Copy) :
    Translate(Copy.Translate), Orientation(Copy.Orientation), Scale(Copy.Scale)
{
}

Transform& Transform::operator=(const Transform& Copy)
{
    Translate = Copy.Translate;
    Orientation = Copy.Orientation;
    Scale = Copy.Scale;
    return *this;
}

RenderableInstance::RenderableInstance(const Renderable *InBaseRenerable) :
    BaseRenderable(InBaseRenerable), InstanceTransform()
{
    if(InBaseRenerable) {
        MaterialOverride.resize(InBaseRenerable->GetSubMeshCount(), nullptr);
    }
}

RenderableInstance::~RenderableInstance() {
    for(auto Material : MaterialOverride) {
        if(Material) {
            delete Material;
        }
    }

    
}

void RenderableInstance::SetMaterial(int IndexSlot, RuntimeMaterial* Material) {
    if(IndexSlot >= 0 && IndexSlot < GetSubMeshCount() && Material != nullptr) {
        if(MaterialOverride[IndexSlot] != nullptr) {
            delete MaterialOverride[IndexSlot];
        }
        MaterialOverride[IndexSlot] = Material;
    }
}

int RenderableInstance::GetSubMeshCount() const {
    if(BaseRenderable) {
        return BaseRenderable->GetSubMeshCount();
    }

    return 0;
}

GLuint RenderableInstance::GetSubMesh(int SlotIndex) const {
    if(BaseRenderable) {
        return BaseRenderable->GetSubMesh(SlotIndex);
    }

    return 0;
}

GLuint RenderableInstance::GetIndexBuffer(int SlotIndex) const {
    if(BaseRenderable) {
        return BaseRenderable->GetIndexBuffer(SlotIndex);
    }

    return 0;
}

GLuint RenderableInstance::GetVertexBuffer() const {
    if(BaseRenderable) {
        return BaseRenderable->GetVertexBuffer();
    }

    return 0;
}

const RuntimeMaterial* RenderableInstance::GetMaterial(int SlotIndex) const {
    if(SlotIndex < GetSubMeshCount() && SlotIndex >= 0) {
        return MaterialOverride[SlotIndex] ? MaterialOverride[SlotIndex] : BaseRenderable->GetMaterial(SlotIndex);
    }

    return nullptr;
}

bool RenderableInstance::IsUseIndex() const {
    if(BaseRenderable) {
        return BaseRenderable->IsUseIndex();
    }

    return false;
}

int RenderableInstance::GetElementCount(int IndexSlot) const {
    return BaseRenderable->GetElementCount(IndexSlot);
}

const Transform* RenderableInstance::GetTransform() const {
    return &InstanceTransform;
}

void RenderableInstance::SetTranslate(const glm::vec3& InTranslate) {
    InstanceTransform.Translate = InTranslate;
}

void RenderableInstance::SetOrientation(const glm::quat& InOrientation) {
    InstanceTransform.Orientation = glm::normalize(InOrientation);
}

void RenderableInstance::SetOrientation(const glm::vec3& Eular) {
    glm::vec3 EularAngle = {glm::radians(Eular.x), glm::radians(Eular.y), glm::radians(Eular.z)};
    glm::quat NewOrientation = glm::quat(EularAngle);
    InstanceTransform.Orientation = NewOrientation;
}

void RenderableInstance::SetScale(const glm::vec3& InScale) {
    InstanceTransform.Scale = InScale;
}
