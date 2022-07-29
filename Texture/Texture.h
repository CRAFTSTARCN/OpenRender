#pragma once
#include "../RHI/GLHandleDef.h"

enum TextureLayoutType {
    Tex2D,
    CubeTexture
};

class Texture {

    GLTextureHandle Handle;
    TextureLayoutType Type;
    uint32_t Width;
    uint32_t Height;
    
public:

    Texture(GLTextureHandle InHandle, TextureLayoutType InType, uint32_t InWidth, uint32_t InHeight);
    ~Texture();

    GLTextureHandle GetTextureHandle() const;
};
