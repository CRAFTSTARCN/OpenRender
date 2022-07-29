#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

enum UniformType {
    
    UniformTypeFloat = 0,
    
    UniformTypeVector4 = 1,
    
    UniformTypeInteger = 2,
    
    UniformTypeIntegerVector4 = 3,
    
    UniformTypeMatrix4_4 = 4,
    
    UniformTypeEnumMax = 5
};

class Uniform {

    constexpr static size_t UniformSizeTable[] = {
        sizeof(float), sizeof(glm::vec4), sizeof(int), sizeof(glm::ivec4), sizeof(glm::mat4)
    };
    
public:

    void* Data;
    UniformType Type;
    bool Immediate;
    
    Uniform(UniformType UType, bool ImmediateUniform = false) : Type(UType), Immediate(ImmediateUniform) {
        assert(UType < UniformTypeEnumMax);
        if(!ImmediateUniform) {
            Data = malloc(UniformSizeTable[UType]);
        }
    }

    Uniform(const Uniform& Copy) : Type(Copy.Type) , Immediate(Copy.Immediate) {
        assert(Copy.Type < UniformTypeEnumMax);
        Data = malloc(UniformSizeTable[Copy.Type]);
        memcpy_s(Data, UniformSizeTable[Copy.Type], Copy.Data, UniformSizeTable[Copy.Type]);
    }

    Uniform(Uniform&& Move) noexcept {
        assert(Move.Type < UniformTypeEnumMax);
        Data = Move.Data;
        Type = Move.Type;
        Immediate = Move.Immediate;
        Move.Data = nullptr;
        Move.Type = UniformTypeEnumMax;
    }

    Uniform& operator=(const Uniform& Copy) {
        if(&Copy == this) {
            return *this;
        }
        if(Data && !Immediate) {
            free(Data);
        }
        assert(Copy.Type < UniformTypeEnumMax);
        Data = malloc(UniformSizeTable[Copy.Type]);
        Immediate = Copy.Immediate;
        memcpy_s(Data, UniformSizeTable[Copy.Type], Copy.Data, UniformSizeTable[Copy.Type]);
        return *this;
    }

    Uniform& operator=(Uniform&& Move) noexcept {
        if(Data && !Immediate) {
            free(Data);
        }
        assert(Move.Type < UniformTypeEnumMax);
        Data = Move.Data;
        Type = Move.Type;
        Immediate = Move.Immediate;
        Move.Data = nullptr;
        Move.Type = UniformTypeEnumMax;
        return *this;
    }
    
    virtual ~Uniform() {
        if(Data && !Immediate) {
            free(Data);
        }
    }
};

class UniformBlock {
    
public:
    void* Data;
    size_t UniformSize;

    UniformBlock(size_t BlockSize) : UniformSize(BlockSize) {
        Data = malloc(UniformSize);
    }
    virtual ~UniformBlock() {
        if(Data) {
            free(Data);
        }
    }
};
