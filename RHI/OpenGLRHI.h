#pragma once
#include <string>
#include <vector>

#include "RenderWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLHandleDef.h"
#include "UniformType.h"
#include "../Mesh/Vertex.hpp"

enum BufferRWType {
    RNW1,
    R1W1,
    RNWN
};

enum TextureParamType {
    WrapS,
    WrapT,
    WrapR,
    MinFilter,
    MagFilter
};

enum TextureWrapType {
    Repeat = 0,
    MirrorRepeat = 1,
    Edge = 2,
    UserDefined = 3
};

enum FilterType {
    Nearest = 0,
    Linear = 1,
    NN = 2,
    NL = 3,
    LN = 4,
    LL = 5
};

enum TextureType {
    RGB = 0,
    sRGB = 1,
    RGBA = 2,
    sRGBA = 3,
    Depth = 4,
    DepthStencil = 5,

    RGB16F = 6,
    

    TextureTypeMax = 7
};

struct TextureParam {
    TextureParamType Type;
    FilterType FilterValue;
    TextureWrapType WrapValue;
};


enum TexDataType {
    UnsignedByte = 0,
    Float = 1,
    INT_24_8 = 2,

    TexDataTypeMax = 3
};

struct TextureData {
    const void* Data;
    TextureType RawDataFormat;
    TexDataType RawDataType;
};

enum DrawMod {
    Triangle = 0,
    Point = 1,
    Line = 2,
    LineStrip = 3,
    LineLoop = 4,
    DrawModMax = 5,
};

enum DepthTestFunction {
    Never = 0,
    Always = 1,
    Less = 2,
    LessEqual = 3,
    Equal = 4,
    GreaterEqual = 5,
    Greater = 6,
    NotEqual = 7,

    DepthFunctionEnumMax = 8
};


struct DrawInfo {

    GLVertexAssembleHandle VAO;
    DrawMod Mod;
    int Offset;
    int Count;

    bool UseElement;
};

class OpenGLRHI {

    GLFWwindow* Window;
    GLShaderHandle GLCreateShaderInternal(const std::string& ShaderText, std::string& OutLog, GLenum ShaderType);

    constexpr static GLenum DrawModTable[] = {
        GL_TRIANGLES, GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP
    };

    constexpr static GLint TextureTypeTable[] = {
        GL_RGB, GL_SRGB, GL_RGBA, GL_SRGB_ALPHA, GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_RGB16F
    };

    constexpr static GLenum TextureDataFormatTable[] = {
        GL_RGB, GL_RGB, GL_RGBA, GL_RGBA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
    };

    constexpr static GLenum TextureDataTypeTable[] = {
        GL_UNSIGNED_BYTE, GL_FLOAT, GL_UNSIGNED_INT_24_8
    };

    constexpr static GLint WrapTypeTable[] = {
        GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    };

    constexpr static GLint FilterTypeTable[] = {
        GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
    };

    constexpr static GLenum DepthTestFunctionTable[] = {
        GL_NEVER, GL_ALWAYS, GL_LESS, GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, GL_NOTEQUAL
    };
    

    struct {
        uint32_t ClearBit;
        GLenum PolygonMod;
        GLenum CullingFace;
        GLenum DepthFunc;

        GLboolean DepthMask;

        bool CullingEnabled;
        bool DepthEnabled;
        bool StencilEnable;

        GLShaderProgramHandle CurrentShader;
        GLFrameBufferHandle CurrentFrameBuffer;
    } PipelineStructInfo;

    std::string WindowName;
    int WindowWidth;
    int WindowHeight;
    
public:

    OpenGLRHI();

    void PreInit();
    void Init();

    void Terminate();

    RenderWindow* GetRenderWindow() const;

    GLShaderHandle CreateVertexShader(const std::string& ShaderText, std::string& OutLog);
    GLShaderHandle CreateFragmentShader(const std::string& ShaderText, std::string& OutLog);
    GLShaderHandle CreateGeometryShader(const std::string& ShaderText, std::string& OutLog);
    GLShaderProgramHandle CreateShaderProgram(const std::vector<GLShaderHandle>& Shaders, std::string& OutLog);
    
    GLVertexAssembleHandle CreateVertexAssemble();
    GLVertexBufferHandle CreateVertexBuffer(int64_t Len, BufferRWType RWType,const  void* Data = nullptr);
    GLIndexBufferHandle CreateIndexBuffer(int64_t Len, BufferRWType RWType, const uint32_t* Data = nullptr);
    GLUniformBufferHandle CreateUniformBuffer(int64_t Len, const void* Data = nullptr);

    void SetUniformBufferData(GLUniformBufferHandle Buffer, int32_t Offset, int64_t Len, const void* Data = nullptr);
    
    GLTextureHandle CreateTexture2D(TextureData Data, uint32_t Width, uint32_t Height, TextureType TexFormat, const std::vector<TextureParam>& Param);
    void GenTexMipmap(GLTextureHandle Texture);
    void SetTexMipmap(GLTextureHandle Texture, int Level, TextureData Data, uint32_t Width, uint32_t Height, TextureType TexFormat);

    GLTextureHandle CreateCubeMapTexture(const std::vector<TextureData>& Data, uint32_t Width, uint32_t Height, TextureType TexFormat, const std::vector<TextureParam>& Param);
    void GenCubeMapMipmap(GLTextureHandle Texture);
    void SetCubeMapMipmap(GLTextureHandle Texture, int Level,const std::vector<TextureData>& Data, uint32_t Width, uint32_t Height, TextureType TexFormat);

    GLFrameBufferHandle CreateFrameBuffer();
    void SetFrameBufferColorAttachment(GLFrameBufferHandle FrameBuffer, GLTextureHandle Attachment, uint32_t Index, uint32_t Mipmap = 0);
    void SetFrameBufferDepthStencilAttachment(GLFrameBufferHandle FrameBuffer, GLTextureHandle Attachment, uint32_t Mipmap = 0);
    void SetFrameBufferDrawCount(GLFrameBufferHandle FrameBuffer, int Count);
    void SetFrameBufferTarget(GLFrameBufferHandle FrameBuffer);
    
    void BindVertexBufferToVertexAssemble(GLVertexAssembleHandle VertexAssemble, GLVertexBufferHandle Buffer, int Stride, uint32_t Index = 0);
    void BindIndexBufferToVertexAssemble(GLVertexAssembleHandle VertexAssemble, GLIndexBufferHandle Buffer);
    void BindUniformBufferToPoint(GLUniformBufferHandle Buffer, int Point);
    
    void SetVertexAssembleAttribute(
        GLVertexAssembleHandle VertexAssemble,
        const std::vector<VertexAttrib>& Attributes,
        uint32_t BufferIndex = 0,
        uint32_t AttribBaseOffset = 0);
    
    void SetVertexBufferData(GLVertexBufferHandle BufferHandle,  int64_t Offset, int64_t Size, const void* Data);
    
    void SetUniform(GLShaderProgramHandle ShaderProgram, const std::string& Name, const Uniform& UniformData);
    void SetUniformTexture(GLShaderProgramHandle ShaderProgram, const std::string& Name, GLTextureHandle Texture, int Index);
    void SetUniformCubeTexture(GLShaderProgramHandle ShaderProgram, const std::string& Name, GLTextureHandle Texture, int Index);
    void SetUniformBlock(GLShaderProgramHandle ShaderProgram, const std::string& Name, int UniformBindingPoint);

    void UseShaderProgram(GLShaderProgramHandle ShaderProgram);
    void DrawVertexAssemble(DrawInfo Info);

    void ClearBuffers();
    void Present();

    void SetViewport(int Width, int Height, int X = 0, int Y = 0);

    void DeleteShader(GLShaderHandle Shader);
    void DeleteShaderProgram(GLShaderProgramHandle ShaderProgram);
    void DeleteVertexAssemble(GLVertexAssembleHandle VertexAssemble);
    void DeleteBuffer(GLuint Buffer);
    void DeleteTexture(GLTextureHandle Texture);
    void DeleteFrameBuffer(GLFrameBufferHandle FrameBuffer);

    void PresetWidthAndHeight(int InWidth, int InHeight);
    void PresetTitle(const std::string& Title);

    void SetDepthFunction(DepthTestFunction FuncType);
};
