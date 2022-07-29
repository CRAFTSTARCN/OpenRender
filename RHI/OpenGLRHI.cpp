#include "OpenGLRHI.h"

#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Texture/Texture.h"


#define GLFW_MAJOR 4
#define GLFW_MINOR 3

GLuint OpenGLRHI::GLCreateShaderInternal(const std::string& ShaderText, std::string& OutLog, GLenum ShaderType) {
    GLShaderHandle Shader =  glCreateShader(ShaderType);
    assert(Shader != 0);
    const char* TextAnsiChar = ShaderText.c_str();
    glShaderSource(Shader,1,&TextAnsiChar,nullptr);
    glCompileShader(Shader);
    int Success = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if(!Success) {
        OutLog.resize(1024);
        glGetShaderInfoLog(Shader, 1024, nullptr, &OutLog[0]);
        glDeleteShader(Shader);
        return 0;
    }
    return Shader;
}

OpenGLRHI::OpenGLRHI() : Window(nullptr), PipelineStructInfo{} {
    
    PipelineStructInfo.ClearBit = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    PipelineStructInfo.PolygonMod = GL_FILL;
    PipelineStructInfo.CullingFace = GL_BACK;
    PipelineStructInfo.DepthFunc = GL_LESS;

    PipelineStructInfo.DepthMask = GL_TRUE;
    
    PipelineStructInfo.CullingEnabled = false;
    PipelineStructInfo.DepthEnabled = true;
    PipelineStructInfo.StencilEnable = false;
    
    PipelineStructInfo.CurrentFrameBuffer = 0;
    PipelineStructInfo.CurrentShader = 0;

    WindowName = "OpenRender";
    WindowWidth = 800;
    WindowHeight = 800;
}

void OpenGLRHI::PreInit() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,GLFW_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,GLFW_MINOR);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    Window = glfwCreateWindow(WindowWidth,WindowHeight,WindowName.c_str(),nullptr,nullptr);
    assert(Window != nullptr);
}

void OpenGLRHI::Init() {
    glfwMakeContextCurrent(Window);
    bool InitGladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(InitGladStatus);

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void OpenGLRHI::Terminate() {
    glfwTerminate();
}

RenderWindow* OpenGLRHI::GetRenderWindow() const {
    static RenderWindow RWindow(WindowWidth, WindowHeight, Window);
    return &RWindow;
}

GLuint OpenGLRHI::CreateVertexShader(const std::string& ShaderText, std::string& OutLog) {
    return GLCreateShaderInternal(ShaderText, OutLog, GL_VERTEX_SHADER);
}

GLuint OpenGLRHI::CreateFragmentShader(const std::string& ShaderText, std::string& OutLog) {
    return GLCreateShaderInternal(ShaderText, OutLog, GL_FRAGMENT_SHADER);
}

GLuint OpenGLRHI::CreateGeometryShader(const std::string& ShaderText, std::string& OutLog) {\
    return GLCreateShaderInternal(ShaderText, OutLog, GL_GEOMETRY_SHADER);
}

GLuint OpenGLRHI::CreateShaderProgram(const std::vector<GLuint>& Shaders, std::string& OutLog) {
    GLShaderProgramHandle ShaderProgram = glCreateProgram();
    assert(ShaderProgram);
    for(auto Shader : Shaders) {
        glAttachShader(ShaderProgram, Shader);
    }
    glLinkProgram(ShaderProgram);
    int Success = 0;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if(!Success) {
        OutLog.resize(1024);
        glGetProgramInfoLog(ShaderProgram, 1024, nullptr, &OutLog[0]);
        glDeleteProgram(ShaderProgram);
        return 0;
    }
    return ShaderProgram;
}

GLuint OpenGLRHI::CreateVertexAssemble() {
    GLVertexAssembleHandle Res = 0;
    glGenVertexArrays(1, &Res);
    assert(Res != 0);
    return Res;
}

GLuint OpenGLRHI::CreateVertexBuffer(int64_t Len, BufferRWType RWType, const void* Data) {
    GLVertexBufferHandle Buffer;
    glGenBuffers(1, &Buffer);
    assert(Buffer != 0);
    GLenum Usage;
    if(RWType == RNWN) {
        Usage = GL_DYNAMIC_DRAW;        
    } else if(RWType == R1W1) {
        Usage = GL_STREAM_DRAW;
    } else {
        Usage = GL_STATIC_DRAW;
    }
    glBindBuffer(GL_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ARRAY_BUFFER, Len, Data, Usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return Buffer;
}

GLuint OpenGLRHI::CreateIndexBuffer(int64_t Len, BufferRWType RWType, const uint32_t* Data) {
    GLVertexBufferHandle Buffer;
    glGenBuffers(1, &Buffer);
    assert(Buffer != 0);
    GLenum Usage;
    if(RWType == RNWN) {
        Usage = GL_DYNAMIC_DRAW;        
    } else if(RWType == R1W1) {
        Usage = GL_STREAM_DRAW;
    } else {
        Usage = GL_STATIC_DRAW;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Len, Data, Usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return Buffer;
}

GLuint OpenGLRHI::CreateUniformBuffer(int64_t Len, const void* Data) {
    GLUniformBufferHandle Buffer = 0;
    glGenBuffers(1, &Buffer);
    assert(Buffer != 0);
    glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
    glBufferData(GL_UNIFORM_BUFFER, Len, Data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return Buffer;
}

void OpenGLRHI::SetUniformBufferData(GLUniformBufferHandle Buffer, int32_t Offset, int64_t Len, const void* Data) {
    assert(Buffer != 0);
    glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, Offset, Len, Data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

GLuint OpenGLRHI::CreateTexture2D(TextureData Data, uint32_t Width, uint32_t Height, TextureType TexFormat, const std::vector<TextureParam>& Param) {
    uint32_t Texture;
    glGenTextures(1,&Texture);
    assert(Texture != 0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    for (auto &TextureParam : Param) {
        GLenum Type;
        GLint Value;
        
        if(TextureParam.Type == WrapS) {
            Type = GL_TEXTURE_WRAP_S;
            Value = WrapTypeTable[TextureParam.WrapValue];
        } else if(TextureParam.Type == WrapT) {
            Type = GL_TEXTURE_WRAP_T;
            Value = WrapTypeTable[TextureParam.WrapValue];
        } else if(TextureParam.Type == MinFilter) {
            Type = GL_TEXTURE_MIN_FILTER;
            Value = FilterTypeTable[TextureParam.FilterValue];
        } else {
            Type = GL_TEXTURE_MAG_FILTER;
            Value = FilterTypeTable[TextureParam.FilterValue];
        }
        glTexParameteri(GL_TEXTURE_2D, Type, Value);
    }

    /*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    */
    

    assert(TexFormat < TextureTypeMax && Data.RawDataFormat < TextureTypeMax && Data.RawDataType < TexDataTypeMax);
    
    glTexImage2D(GL_TEXTURE_2D,
        0,
        TextureTypeTable[TexFormat],
        (int)Width, (int)Height,
        0,
        TextureDataFormatTable[Data.RawDataFormat],
        TextureDataTypeTable[Data.RawDataType],
        Data.Data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return Texture;
}

void OpenGLRHI::GenTexMipmap(GLuint Texture) {
    assert(Texture != 0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRHI::SetTexMipmap(GLuint Texture, int Level, TextureData Data, uint32_t Width, uint32_t Height,
    TextureType TexFormat) {
    assert(Texture != 0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    assert(TexFormat < TextureTypeMax && Data.RawDataFormat < TextureTypeMax && Data.RawDataType < TexDataTypeMax);
    glTexImage2D(GL_TEXTURE_2D,
       Level,
       TextureTypeTable[TexFormat],
       Width, Height,
       0,
       TextureDataFormatTable[Data.RawDataFormat],
       TextureDataTypeTable[Data.RawDataType],
       Data.Data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint OpenGLRHI::CreateCubeMapTexture(const std::vector<TextureData>& Data, uint32_t Width, uint32_t Height,
    TextureType TexFormat, const std::vector<TextureParam>& Param) {
    GLTextureHandle CubeMap = 0;
    glGenTextures(1, &CubeMap);
    assert(CubeMap != 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);

    for (auto &TextureParam : Param) {
        GLenum Type;
        GLint Value;
        if(TextureParam.Type == WrapS) {
            Type = GL_TEXTURE_WRAP_S;
            Value = WrapTypeTable[TextureParam.WrapValue];
        } else if(TextureParam.Type == WrapT) {
            Type = GL_TEXTURE_WRAP_T;
            Value = WrapTypeTable[TextureParam.WrapValue];
        } else if(TextureParam.Type == WrapR) {
            Type = GL_TEXTURE_WRAP_R;
            Value = WrapTypeTable[TextureParam.WrapValue];
        }else if(TextureParam.Type == MinFilter) {
            Type = GL_TEXTURE_MIN_FILTER;
            Value = GL_LINEAR_MIPMAP_LINEAR;
        } else {
            Type = GL_TEXTURE_MAG_FILTER;
            Value = FilterTypeTable[TextureParam.FilterValue];
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, Type, Value);
    }

    assert(TexFormat < TextureTypeMax);

    for(int i=0; i<6 && i<Data.size(); ++i) {
        assert(Data[i].RawDataFormat < TextureTypeMax && Data[i].RawDataType < TexDataTypeMax);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            TextureTypeTable[TexFormat],
            (int)Width, (int)Height,
            0,
            TextureDataFormatTable[Data[i].RawDataFormat],
            TextureDataTypeTable[Data[i].RawDataType],
            Data[i].Data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return CubeMap;
}

void OpenGLRHI::GenCubeMapMipmap(GLuint Texture) {
    assert(Texture != 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLRHI::SetCubeMapMipmap(GLuint Texture, int Level, const std::vector<TextureData>& Data, uint32_t Width,
    uint32_t Height, TextureType TexFormat) {
    
    assert(Texture != 0);
    assert(TexFormat < TextureTypeMax);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
    
    for(int i=0; i<6 && i<Data.size(); ++i) {
        assert(Data[i].RawDataFormat < TextureTypeMax && Data[i].RawDataType < TexDataTypeMax);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
           Level,
           TextureTypeTable[TexFormat],
           (int)Width, (int)Height,
           0,
           TextureDataFormatTable[Data[i].RawDataFormat],
           TextureDataTypeTable[Data[i].RawDataType],
           Data[i].Data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
}

GLuint OpenGLRHI::CreateFrameBuffer() {
    GLFrameBufferHandle FrameBuffer;
    glGenFramebuffers(1, &FrameBuffer);
    assert(FrameBuffer != 0); //Notice that '0' is default frame buffer gen by system
    return FrameBuffer;
}

void OpenGLRHI::SetFrameBufferColorAttachment(GLuint FrameBuffer, GLuint Attachment, uint32_t Index, uint32_t Mipmap) {
    assert(Index < 31);
    assert(Attachment > 0);

    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GL_TEXTURE_2D, Attachment, Mipmap);

    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, PipelineStructInfo.CurrentFrameBuffer);
    }
}

void OpenGLRHI::SetFrameBufferDepthStencilAttachment(GLuint FrameBuffer, GLuint Attachment, uint32_t Mipmap) {
    assert(Attachment > 0);

    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, Attachment, Mipmap);

    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, PipelineStructInfo.CurrentFrameBuffer);
    }
}

void OpenGLRHI::SetFrameBufferDrawCount(GLuint FrameBuffer, int Count) {

    assert(Count < 32);
    
    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    }

    if(Count == 0) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else {
        static std::vector<GLenum> DrawBuffers = []() 
        {
            std::vector<GLenum> Res(32);
            for(uint32_t i=0; i<32; ++i) {
                Res[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            return Res;
        }();

        glDrawBuffers(Count, DrawBuffers.data());
    }

    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, PipelineStructInfo.CurrentFrameBuffer);
    }
}

void OpenGLRHI::SetFrameBufferTarget(GLuint FrameBuffer) {
    if(PipelineStructInfo.CurrentFrameBuffer != FrameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        PipelineStructInfo.CurrentFrameBuffer = FrameBuffer;
    } 
}

void OpenGLRHI::BindVertexBufferToVertexAssemble(GLuint VertexAssemble, GLuint Buffer, int Stride, uint32_t Index) {
    assert(VertexAssemble !=0  && Buffer != 0);
    glBindVertexArray(VertexAssemble);
    glBindVertexBuffer(Index, Buffer, 0, Stride);
    glBindVertexArray(0);
}

void OpenGLRHI::BindIndexBufferToVertexAssemble(GLuint VertexAssemble, GLuint Buffer) {
    assert(VertexAssemble != 0 && Buffer != 0);
    glBindVertexArray(VertexAssemble);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLRHI::BindUniformBufferToPoint(GLuint Buffer, int Point) {
    assert(Buffer != 0);
    glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, Point, Buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLRHI::SetVertexAssembleAttribute(GLuint VertexAssemble, const std::vector<VertexAttrib>& Attributes,
                                           uint32_t BufferIndex, uint32_t AttribBaseOffset)
{
    assert(VertexAssemble != 0);
    glBindVertexArray(VertexAssemble);
    for(const auto& Attrib : Attributes) {
        uint32_t RealIndex = Attrib.Index + AttribBaseOffset;
        glVertexAttribFormat(RealIndex, (int)Attrib.Size, GL_FLOAT, GL_FALSE, Attrib.Offset);
        glVertexAttribBinding(RealIndex, BufferIndex);
        glEnableVertexAttribArray(RealIndex);
    }
    glBindVertexArray(0);
}

void OpenGLRHI::SetVertexBufferData(GLuint BufferHandle, int64_t Offset, int64_t Size, const void* Data) {
    assert(BufferHandle != 0);
    glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
    glBufferSubData(GL_ARRAY_BUFFER, Offset, Size, Data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLRHI::SetUniform(GLuint ShaderProgram, const std::string& Name, const Uniform& UniformData) {
    assert(ShaderProgram != 0);
    int Location = glGetUniformLocation(ShaderProgram, Name.c_str());

    if(Location < 0) {
        return;
    }
    
    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(ShaderProgram);
    }

    glm::vec4* DataPtr = nullptr;
    glm::ivec4* DataIPtr = nullptr;
    glm::mat4* DataMPtr = nullptr;
    
    switch (UniformData.Type) {
    
    case UniformTypeFloat:
        glUniform1f(Location, *((float*)UniformData.Data));
        break;
    case UniformTypeVector4:
        DataPtr = static_cast<glm::vec4*>(UniformData.Data);
        glUniform4fv(Location, 1, glm::value_ptr(*DataPtr));
        break;

    case UniformTypeInteger:
        glUniform1i(Location, *((int*)UniformData.Data));
        break;

    case UniformTypeIntegerVector4:
        DataIPtr = static_cast<glm::ivec4*>(UniformData.Data);
        glUniform4iv(Location, 1, glm::value_ptr(*DataIPtr));
        break;

    case UniformTypeMatrix4_4:
        DataMPtr = static_cast<glm::mat4*>(UniformData.Data);
        glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(*DataMPtr));
        break;
        
    default:
        break;
    }

    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(PipelineStructInfo.CurrentShader);
    }
}

void OpenGLRHI::SetUniformTexture(GLuint ShaderProgram, const std::string& Name, GLuint Texture, int Index) {
    assert(ShaderProgram != 0 && Texture != 0 && Index >= 0);

    glActiveTexture(GL_TEXTURE0+Index);
    glBindTexture(GL_TEXTURE_2D, Texture);
    
    int Location = glGetUniformLocation(ShaderProgram, Name.c_str());
    if(Location < 0) {
        return;
    }
    

    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(ShaderProgram);
    }

    glUniform1i(Location, Index);

    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(PipelineStructInfo.CurrentShader);
    }
    
}

void OpenGLRHI::SetUniformCubeTexture(GLuint ShaderProgram, const std::string& Name, GLuint Texture, int Index) {
    assert(ShaderProgram != 0 && Texture != 0 && Index >= 0);
    
    glActiveTexture(GL_TEXTURE0+Index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
    
    int Location = glGetUniformLocation(ShaderProgram, Name.c_str());
    if(Location < 0) {
        return;
    }
    

    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(ShaderProgram);
    }

    glUniform1i(Location, Index);

    if(PipelineStructInfo.CurrentShader != ShaderProgram) {
        glUseProgram(PipelineStructInfo.CurrentShader);
    }
}

void OpenGLRHI::SetUniformBlock(GLuint ShaderProgram, const std::string& Name, int UniformBindingPoint) {
    int Location = glGetUniformBlockIndex(ShaderProgram, Name.c_str());
    if(Location >= 0) {
        glUniformBlockBinding(ShaderProgram, Location, UniformBindingPoint);
    }
}

void OpenGLRHI::UseShaderProgram(GLuint ShaderProgram) {
    if(PipelineStructInfo.CurrentShader == ShaderProgram) {
        return;
    }

    glUseProgram(ShaderProgram);
    PipelineStructInfo.CurrentShader = ShaderProgram;
}

void OpenGLRHI::DrawVertexAssemble(DrawInfo Info) {
    assert(Info.VAO);
    assert(Info.Mod < DrawModMax);
    GLenum Mod = DrawModTable[Info.Mod];
    glBindVertexArray(Info.VAO);
    if(Info.UseElement) {
        glDrawElements(Mod, Info.Count, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(Mod, Info.Offset, Info.Count);
    }
    glBindVertexArray(0);
}

void OpenGLRHI::ClearBuffers() {
    glClear(PipelineStructInfo.ClearBit);
}

void OpenGLRHI::Present() {
    glfwSwapBuffers(Window);
}

void OpenGLRHI::SetViewport(int InWidth, int InHeight, int X, int Y) {
    WindowWidth = InWidth;
    WindowHeight = InHeight;
    glViewport(X, Y, InWidth, InHeight);
}

void OpenGLRHI::DeleteShader(GLuint Shader) {
    glDeleteShader(Shader);
}

void OpenGLRHI::DeleteShaderProgram(GLuint ShaderProgram) {
    glDeleteProgram(ShaderProgram);
}

void OpenGLRHI::DeleteVertexAssemble(GLuint VertexAssemble) {
    glDeleteVertexArrays(1, &VertexAssemble);
}

void OpenGLRHI::DeleteBuffer(GLuint Buffer) {
    glDeleteBuffers(1, &Buffer);
}

void OpenGLRHI::DeleteTexture(GLuint Texture) {
    glDeleteTextures(1, &Texture);
}

void OpenGLRHI::DeleteFrameBuffer(GLuint FrameBuffer) {
    glDeleteFramebuffers(1, &FrameBuffer);
}

void OpenGLRHI::PresetWidthAndHeight(int InWidth, int InHeight) {
    WindowWidth = InWidth;
    WindowHeight = InHeight;
}

void OpenGLRHI::PresetTitle(const std::string& Title) {\
    WindowName = Title;
}

void OpenGLRHI::SetDepthFunction(DepthTestFunction FuncType) {
    if(FuncType == DepthFunctionEnumMax) {
        return;
    }

    GLenum GLDepthFunctionType = DepthTestFunctionTable[FuncType];
    if(GLDepthFunctionType == PipelineStructInfo.DepthFunc) {
        return;
    }

    PipelineStructInfo.DepthFunc = GLDepthFunctionType;
    glDepthFunc(GLDepthFunctionType);
}
