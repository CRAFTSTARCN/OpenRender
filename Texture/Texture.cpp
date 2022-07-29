#include "Texture.h"

Texture::Texture(GLuint InHandle, TextureLayoutType InType, uint32_t InWidth, uint32_t InHeight) :
    Handle(InHandle), Type(InType), Width(InWidth), Height(InHeight)
{
}

Texture::~Texture() {
}

GLuint Texture::GetTextureHandle() const {
    return Handle;
}
