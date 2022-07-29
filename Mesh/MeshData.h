#pragma once
#include <vector>

#include "Vertex.hpp"

struct MeshData {
    std::vector<StaticMeshVertex> Vertices;
    std::vector<std::vector<unsigned int>> Indices;
};
