#pragma once

#include <vector>
#include <glm/glm.hpp>

struct VertexAttrib {
    uint32_t Index;
    uint32_t Size;
    uint32_t Offset;
    uint32_t Stride;
};


template <unsigned int UVCount>
struct StaticMeshUniVertex {
    
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec2 UV[UVCount];
};

typedef StaticMeshUniVertex<1> StaticMeshVertex;

template <typename VertexType>
class AttribGetter {
    
protected:
    AttribGetter() {}

public:
    const std::vector<VertexAttrib>& GetAttrib() const { return {}; }
    
};

template <unsigned int UVCount>
class AttribGetter<StaticMeshUniVertex<UVCount>> {
    
public:
    AttribGetter() {}
    const std::vector<VertexAttrib>& GetAttrib() const {
        static std::vector<VertexAttrib> Attrib = []()
        {
            std::vector<VertexAttrib> Res(3+UVCount);
            Res[0] = {0, 3, 0, sizeof(StaticMeshUniVertex<UVCount>)};
            Res[1] = {1, 3, offsetof(StaticMeshUniVertex<UVCount>, Normal), sizeof(StaticMeshUniVertex<UVCount>)};
            Res[2] = {2, 3, offsetof(StaticMeshUniVertex<UVCount>, Tangent), sizeof(StaticMeshUniVertex<UVCount>)};

            for(uint32_t i = 0; i<UVCount; ++i) {
                Res[i+3] = {i+3,
                    2,
                    offsetof(StaticMeshUniVertex<UVCount>, UV + i * sizeof(glm::vec2)),
                    sizeof(StaticMeshUniVertex<UVCount>)};
            }
            return Res;
        }();

        return Attrib;
    }
};

