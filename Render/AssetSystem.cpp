#include  <json11/json11.hpp>

#include "AssetSystem.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Util/Logger.h"

#define CHECK_NAME(JSONFILE, NAME_KEY) std::string Name = (JSONFILE)[NAME_KEY].string_value(); if(Name.empty()) return false;

#define CHECK_ITER(NAME, ITER_NAME, FROM, OVERRIDE) \
    auto (ITER_NAME) = (FROM).find(NAME); \
    if((ITER_NAME) != (FROM).end() && !(OVERRIDE)) { \
        return false; \
    }

#define UNI_GET_FUNC(FromMap, Key) auto Iter = (FromMap).find(Key); \
    if(Iter != (FromMap).end()) { \
        return Iter->second; \
    }\
    \
    return nullptr; 

using json11::Json;

void AssetSystem::ParseParamFromJson(MaterialParam& PushTo, const Json& JsonObject) {
    std::string Name = JsonObject["Name"].string_value();
    if(Name.empty()) {
        return;
    }

    int Type = JsonObject["Type"].int_value();
    if(Type == -1) {

        std::string TextureName = JsonObject["Val"].string_value();
        auto Iter = Textures.find(TextureName);
        if(Iter != Textures.end()) {
            PushTo.AddTextureParam(Name, Iter->second);
        }
        
    } else {
        if(Type == 0) {
            bool Success = PushTo.AddParam(Name, UniformTypeFloat);
            if(Success) {
                float Val = (float)JsonObject["Val"].number_value();
                PushTo.SetScalarParam(Name, Val);
            }
        } else if(Type == 1) {
            bool Success = PushTo.AddParam(Name, UniformTypeVector4);
            if(Success) {
                glm::vec4 Vector{};
                auto Val = JsonObject["Val"];
                Vector.x = (float)Val["x"].number_value();
                Vector.y = (float)Val["y"].number_value();
                Vector.z = (float)Val["z"].number_value();
                Vector.w = (float)Val["w"].number_value();

                PushTo.SetVectorParam(Name, Vector);
            }
            
        } else if(Type == 2) {
            
        } else if(Type == 3) {
            
        } else if(Type == 4) {
            
        } 
    }
    
}

void AssetSystem::ProcessMeshData(MeshData& ToData, const aiScene* Scene, aiNode* Node) {
    for(size_t i=0; i<Node->mNumMeshes; ++i) {
        aiMesh *Mesh = Scene->mMeshes[Node->mMeshes[i]];
        ProcessMesh(ToData, Mesh);
    }

    for(size_t i=0; i<Node->mNumChildren; ++i) {
        ProcessMeshData(ToData, Scene, Node->mChildren[i]);
    }
}

void AssetSystem::ProcessMesh(MeshData& ToData, aiMesh* Mesh) {
    unsigned int CurrentVertexOffset = (unsigned int)ToData.Vertices.size();
    auto Position = Mesh->mVertices;
    auto Normal = Mesh->mNormals;
    auto Tangent = Mesh->mTangents;
    auto UV = Mesh->mTextureCoords[0];
    
    unsigned int CurrentVertexIndex = CurrentVertexOffset;
    for(size_t i=0; i<Mesh->mNumVertices; ++i) {
        ToData.Vertices.push_back({});
        StaticMeshVertex& Vertex = ToData.Vertices[CurrentVertexIndex];

        
        Vertex.Position = {Position[i].x, Position[i].y, Position[i].z};
        
        if(Normal) {
            Vertex.Normal = {Normal[i].x, Normal[i].y, Normal[i].z};
        } else {
            Vertex.Normal = {0, 1, 0};
        }

        if(Tangent) {
            Vertex.Tangent = {Tangent[i].x, Tangent[i].y, Tangent[i].z};
        } else {
            Vertex.Tangent = {1, 0, 0};
        }

        if(UV) {
            Vertex.UV[0] = {UV[i].x, UV[i].y};
        }
        
        ++CurrentVertexIndex;
    }

    auto Faces = Mesh->mFaces;
    std::vector<unsigned int> Indices;
    for(unsigned int i=0; i<Mesh->mNumFaces; ++i) {
        aiFace& Face = Faces[i];

        for(unsigned int j=0; j<Face.mNumIndices; ++j) {
            Indices.push_back(Face.mIndices[j] + CurrentVertexOffset);
        }
        
    }
    ToData.Indices.push_back(std::move(Indices));
}

void AssetSystem::DestroyRenderableAsset(OpenGLRHI *RHI, Renderable* Model) {
    for(int i=0; i<Model->GetSubMeshCount(); ++i) {
        RHI->DeleteVertexAssemble(Model->GetSubMesh(i));
        if(Model->IsUseIndex()) {
            RHI->DeleteBuffer(Model->GetIndexBuffer(i));
        }
    }

    RHI->DeleteBuffer(Model->GetVertexBuffer());
}

AssetSystem::AssetSystem() {
}

AssetSystem::~AssetSystem() {
}

std::string AssetSystem::LoadTextFileToStr(const std::string& FilePath) {
    char Buffer[1024];
    FILE* File = nullptr;
    fopen_s(&File, FilePath.c_str(), "rt");

    if(!File) return "";

    std::string Content;
    size_t ReadSize;
    while (true) {
        ReadSize = fread_s(Buffer, 1024, sizeof(char), 1024, File);
        Content.append(Buffer, ReadSize);
        if(ReadSize < 1024) {
            break;
        }
    }

    fclose(File);
    return Content;
}

void AssetSystem::ClearAll(OpenGLRHI* RHI) {
    ClearShader(RHI);
    ClearAllRenderables(RHI);
    ClearAllMaterials(RHI);
    ClearAllShaderProgram(RHI);
    ClearAllTextures(RHI);
    
}

void AssetSystem::ClearShader(OpenGLRHI* RHI) {
    for(auto &Shader : Shaders) {
        RHI->DeleteShader(Shader.second);
    }

    std::unordered_map<std::string, GLShaderHandle>().swap(Shaders);
}

void AssetSystem::ClearAllShaderProgram(OpenGLRHI* RHI) {
    for(auto ShaderProgram : ShaderPrograms) {
        RHI->DeleteShaderProgram(ShaderProgram.second);
    }

    std::unordered_map<std::string, GLShaderProgramHandle>().swap(ShaderPrograms);
}

void AssetSystem::ClearAllRenderables(OpenGLRHI* RHI) {
    for(auto& Model : Renderables) {
        DestroyRenderableAsset(RHI, Model.second);
    }
    
    std::unordered_map<std::string, Renderable*>().swap(Renderables);
}

void AssetSystem::ClearAllMaterials(OpenGLRHI* RHI) {
    for(auto Mat : Materials) {
        delete Mat.second;
    }

    std::unordered_map<std::string, Material*>().swap(Materials);
    
}

void AssetSystem::ClearAllTextures(OpenGLRHI* RHI) {
    for(auto Tex : Textures) {
        RHI->DeleteTexture(Tex.second->GetTextureHandle());
        delete Tex.second;
    }

    std::unordered_map<std::string, Texture*>().swap(Textures);
}

bool AssetSystem::LoadVertexShader(OpenGLRHI* RHI, const std::string& Name, const std::string& Path,
                                   bool ExistOverride) {

    auto Iter = Shaders.find(Name);
    if(Iter != Shaders.end() && !ExistOverride) {
        return false;
    }

    std::string ShaderCode = LoadTextFileToStr(Path);
    if(ShaderCode.empty()) {
        return false;
    }

    std::string Log;
    GLShaderHandle Shader = RHI->CreateVertexShader(ShaderCode, Log);
    if(Shader == 0) {
        Logger::Get().Error(Log.c_str(), "VertexShaderError: " + Name);
        return false;
    }

    if(Iter != Shaders.end()) {
        RHI->DeleteShader(Iter->second);
    }
    
    Shaders[Name] = Shader;
    return true;
}

bool AssetSystem::LoadFragmentShader(OpenGLRHI* RHI, const std::string& Name, const std::string& Path,
    bool ExistOverride) {
    auto Iter = Shaders.find(Name);
    if(Iter != Shaders.end() && !ExistOverride) {
        return false;
    }

    std::string ShaderCode = LoadTextFileToStr(Path);
    if(ShaderCode.empty()) {
        return false;
    }

    std::string Log;
    GLShaderHandle Shader = RHI->CreateFragmentShader(ShaderCode, Log);
    if(Shader == 0) {
        Logger::Get().Error(Log.c_str(), "FragmentShaderError: " + Name);
        return false;
    }

    if(Iter != Shaders.end()) {
        RHI->DeleteShader(Iter->second);
    }
    
    Shaders[Name] = Shader;
    return true;
}

bool AssetSystem::LoadShaderProgram(OpenGLRHI* RHI, const std::string& Path, bool ExistOverride) {

    std::string Log;
    Json FileJson = Json::parse(LoadTextFileToStr(Path), Log);
    if(FileJson.is_null()) {
        return false;
    }

    CHECK_NAME(FileJson, "Name")

    CHECK_ITER(Name, ShaderProgramIter, ShaderPrograms, ExistOverride)

    auto ShaderList = FileJson["Shaders"].array_items();
    if(ShaderList.empty()) {
        return false;
    }
    
    std::vector<GLShaderHandle> ShaderHandleList;
    auto ShaderEnd = Shaders.end();
    for(auto &ShaderName : ShaderList) {
        auto Iter = Shaders.find(ShaderName.string_value());
        if(Iter == ShaderEnd) {
            return false;
        } else {
            ShaderHandleList.push_back(Iter->second);
        }
    }

    Log.clear();
    GLShaderProgramHandle ShaderProgram = RHI->CreateShaderProgram(ShaderHandleList, Log);
    if(ShaderProgram == 0) {
        Logger::Get().Error(Log.c_str(), "ShaderProgramLinkError: " + Name);
        return false;
    }
    
    if(ShaderProgramIter != ShaderPrograms.end()) {
        RHI->DeleteShaderProgram(ShaderProgram);
    }

    ShaderPrograms[Name] = ShaderProgram;
    return true;
}

bool AssetSystem::LoadTexture(OpenGLRHI* RHI, const std::string& Name, const std::string& Path, bool SRGB, bool ExistOverride) {
    CHECK_ITER(Name, Iter, Textures, ExistOverride)

    int Width, Height, Channels;
    unsigned char * Result = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);
    if(Result == nullptr) {
        return false;
    }

    
    TextureData Data = {Result};
    TextureType APITexType;

    if(Channels == 4) {
        Data.RawDataFormat = RGBA;
        APITexType = SRGB ? sRGBA : RGBA;
    } else {
        Data.RawDataFormat = RGB;
        APITexType = SRGB ? sRGB : RGB;

    }

    Data.RawDataType = UnsignedByte;
    
    GLTextureHandle TextureHandle = RHI->CreateTexture2D(Data, Width, Height, APITexType, {
        {MinFilter, Linear}, {MagFilter, Linear}, {WrapS, Linear, Repeat}, {WrapT, Linear, Repeat} //TBD
    });
    stbi_image_free(Result);
    Texture* NewTexture = new Texture(TextureHandle, Tex2D ,Width, Height);

    if(Iter != Textures.end()) {
        RHI->DeleteTexture(Iter->second->GetTextureHandle());
        delete Iter->second;
    }

    Textures[Name] = NewTexture;
    return true;
}

bool AssetSystem::LoadTexture(OpenGLRHI* RHI, const std::string& Path, bool ExistOverride) {
    return true;
}

bool AssetSystem::LoadTextureHDR(OpenGLRHI* RHI, const std::string& Name, const std::string& Path, bool ExistOverride) {
    CHECK_ITER(Name, Iter, Textures, ExistOverride)

    int Width, Height, Channels;
    float * Result = stbi_loadf(Path.c_str(), &Width, &Height, &Channels, 0);
    if(Result == nullptr) {
        return false;
    }
    
    TextureData Data = {Result, RGB, Float};
    if(Channels == 4) {
        Data.RawDataFormat = RGBA;
    }
    
    GLTextureHandle TextureHandle = RHI->CreateTexture2D(Data, Width, Height, RGB16F, {
        {MinFilter, Linear}, {MagFilter, Linear}, {WrapS, Linear, Repeat}, {WrapT, Linear, Repeat}
    });
    stbi_image_free(Result);

    Texture* NewTexture = new Texture(TextureHandle, Tex2D ,Width, Height);

    if(Iter != Textures.end()) {
        RHI->DeleteTexture(Iter->second->GetTextureHandle());
        delete Iter->second;
    }

    Textures[Name] = NewTexture;
    return true;
}

bool AssetSystem::LoadHDRCube(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir, bool ExistOverride) {
    std::string Str = LoadTextFileToStr(Path);
    if(Str.empty()) {
        return false;
    }
    std::string Log;
    Json FileJson = Json::parse(Str, Log);
    std::string Name = FileJson["Name"].string_value();

    if(Name.empty()) {
        return false;
    }

    auto Iter = Textures.find(Name);
    if(Iter != Textures.end() && !ExistOverride) {
        return false;
    }

    auto Faces = FileJson["Faces"].array_items();

    std::vector<TextureData> FaceData;

    int Width = 0, Height = 0, Channels;
    for(auto& FacePath : Faces) {
        int OldWidth = Width, OldHeight = Height;
        std::filesystem::path RealPath = BaseDir / FacePath.string_value();
        float * Result = stbi_loadf(RealPath.generic_string().c_str(), &Width, &Height, &Channels, 0);
        
        if(Result == nullptr) {
            return false;
        }
        
        TextureData Data = {Result, RGB ,Float};


        if(Channels == 4) {
            Data.RawDataFormat = RGBA;
        }

        if((OldWidth != Width || OldHeight != Height) && OldWidth != 0 && OldHeight != 0) {
            return false;
        }
        FaceData.push_back(Data);
    }

    GLTextureHandle Tex = RHI->CreateCubeMapTexture(FaceData, Width, Height, RGB16F,{
        {MinFilter, Linear}, {MagFilter, Linear}, {WrapS, Linear, Edge}, {WrapT, Linear, Edge}, {WrapR, Linear, Edge}
    });
    for(auto &Face : FaceData) {
        stbi_image_free(const_cast<void*>(Face.Data));
    }

    bool AutoLod = FileJson["AutoLod"].bool_value();
    if(AutoLod) {
        RHI->GenCubeMapMipmap(Tex);
    } else {
        auto Lods = FileJson["Lods"].array_items();
        int Level = 1;
        for(auto &Lod : Lods) {
            FaceData.clear();
            auto LodFaces = Lod.array_items();
            int LodWidth = 0, LodHeight = 0;
            for(auto& FacePath : LodFaces) {
                int OldWidth = LodWidth, OldHeight = LodHeight;
                std::filesystem::path RealPath = BaseDir / FacePath.string_value();
                float * Result = stbi_loadf(RealPath.generic_string().c_str(), &LodWidth, &LodHeight, &Channels, 0);
        
                if(Result == nullptr) {
                    return false;
                }

                if((OldWidth != LodWidth || OldHeight != LodWidth) && OldWidth != 0 && OldHeight != 0) {
                    return false;
                }
                TextureData Data = {Result, RGB16F ,Float};
                FaceData.push_back(Data);
            }

            RHI->SetCubeMapMipmap(Tex, Level, FaceData, LodWidth, LodHeight, RGB16F);
            ++Level;
            for(auto &Face : FaceData) {
                stbi_image_free(const_cast<void*>(Face.Data));
            }
        }
    }

    if(Iter != Textures.end()) {
        RHI->DeleteTexture(Iter->second->GetTextureHandle());
        delete Iter->second;
    }

    Textures[Name] = new Texture(Tex, CubeTexture, Width, Height);
    return true;
}

bool AssetSystem::LoadCube(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir,
     bool SRGB, bool ExistOverride) {
    std::string Str = LoadTextFileToStr(Path);
    if(Str.empty()) {
        return false;
    }
    std::string Log;
    Json FileJson = Json::parse(Str, Log);
    
    CHECK_NAME(FileJson, "Name")
    
    CHECK_ITER(Name, Iter, Textures, ExistOverride)
    
    auto Faces = FileJson["Faces"].array_items();

    std::vector<TextureData> FaceData;

    int Width = 0, Height = 0, Channels;

    bool IsSRGBTexture = FileJson["sRGB"].bool_value();
    TextureType APITexType = IsSRGBTexture ? sRGB : RGB;


    for(auto& FacePath : Faces) {
        int OldWidth = Width, OldHeight = Height;
        std::filesystem::path RealPath = BaseDir / FacePath.string_value();
        unsigned char * Result = stbi_load(RealPath.generic_string().c_str(), &Width, &Height, &Channels, 0);
        
        if(Result == nullptr) {
            return false;
        }

        if((OldWidth != Width || OldHeight != Height) && OldWidth != 0 && OldHeight != 0) {
            return false;
        }

        TextureData Data = {Result};

        if(Channels == 4) {
            Data.RawDataFormat = RGBA;
            APITexType = IsSRGBTexture ? sRGBA : RGBA;
        } else {
            Data.RawDataFormat = RGB;
        }
        
        Data.RawDataType = UnsignedByte;
        FaceData.push_back(Data);
    }

    GLTextureHandle Tex = RHI->CreateCubeMapTexture(FaceData, Width, Height, APITexType,{
        {MinFilter, Linear}, {MagFilter, Linear}, {WrapS, Linear, Repeat}, {WrapT, Linear, Repeat}, {WrapR, Linear, Edge}
    });
    for(auto &Face : FaceData) {
        stbi_image_free(const_cast<void*>(Face.Data));
    }

    bool AutoLod = FileJson["AutoLod"].bool_value();
    if(AutoLod) {
        RHI->GenCubeMapMipmap(Tex);
    } else {
        RHI->GenCubeMapMipmap(Tex);
        auto Lods = FileJson["Lods"].array_items();
        int Level = 1;
        for(auto &Lod : Lods) {
            FaceData.clear();
            auto LodFaces = Lod.array_items();
            int LodWidth = 0, LodHeight = 0;
            for(auto& FacePath : LodFaces) {
                int OldWidth = LodWidth, OldHeight = LodHeight;
                std::filesystem::path RealPath = BaseDir / FacePath.string_value();
                unsigned char * Result = stbi_load(RealPath.generic_string().c_str(), &LodWidth, &LodHeight, &Channels, 0);
        
                if(Result == nullptr) {
                    return false;
                }

                if((OldWidth != LodWidth || OldHeight != LodWidth) && OldWidth != 0 && OldHeight != 0) {
                    return false;
                }
                TextureData Data = {Result};
                if(Channels == 4) {
                    Data.RawDataFormat = RGBA;
                } else {
                    Data.RawDataFormat = RGB;
                }
        
                Data.RawDataType = UnsignedByte;
                FaceData.push_back(Data);
            }

            RHI->SetCubeMapMipmap(Tex, Level, FaceData, LodWidth, LodHeight, APITexType);
            ++Level;
            for(auto &Face : FaceData) {
                stbi_image_free(const_cast<void*>(Face.Data));
            }
        }
    }

    if(Iter != Textures.end()) {
        RHI->DeleteTexture(Iter->second->GetTextureHandle());
        delete Iter->second;
    }

    Textures[Name] = new Texture(Tex, CubeTexture, Width, Height);
    return true;
}

bool AssetSystem::LoadMaterial(const std::string& Path, bool ExistOverride) {
    std::string Str = LoadTextFileToStr(Path);
    if(Str.empty()) {
        return false;
    }
    std::string Log;
    Json FileJson = Json::parse(Str, Log);
    
    CHECK_NAME(FileJson, "Name")
    
    CHECK_ITER(Name, Iter, Materials, ExistOverride)
    

    std::string ShaderName = FileJson["ShaderName"].string_value();
    auto ShaderIter = ShaderPrograms.find(ShaderName);
    if(ShaderIter == ShaderPrograms.end()) {
        return false;
    }

    int LightWeight = FileJson["LightWeight"].int_value();
    if(LightWeight <= 0) {
        LightWeight = 1;
    }

    bool UseIBL = FileJson["UseIBL"].bool_value();
    bool DoubleSided = FileJson["DoubleSided"].bool_value();

    MaterialParam InitialParam;

    auto ParamList = FileJson["ParamList"].array_items();
    for(auto& Param : ParamList) {
        ParseParamFromJson(InitialParam, Param);
    }

    Material *Mat = new Material(ShaderIter->second, LightWeight, UseIBL, DoubleSided);
    Mat->SetDefaultParam(std::move(InitialParam));

    if(Iter != Materials.end()) {
        delete Iter->second;
    }

    Materials[Name] = Mat;
    return true;
}

bool AssetSystem::LoadRenderable(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir, bool ExistOverride) {
    std::string Str = LoadTextFileToStr(Path);
    if(Str.empty()) {
        return false;
    }
    std::string Log;
    Json FileJson = Json::parse(Str, Log);
    CHECK_NAME(FileJson, "Name")

    CHECK_ITER(Name, Iter, Renderables, ExistOverride)
    

    std::string AssetPath = FileJson["Path"].string_value();
    if(AssetPath.empty()) {
        return false;
    }

    AssetPath = (BaseDir / AssetPath).generic_string();
    
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(AssetPath.c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if(!Scene || !Scene->mRootNode) {
        return false;
    }

    MeshData Data{};
    ProcessMeshData(Data, Scene, Scene->mRootNode);

    GLVertexBufferHandle VertexBuffer = RHI->CreateVertexBuffer(
        Data.Vertices.size() * sizeof(StaticMeshVertex),
        RNW1,
        Data.Vertices.data()
        );

    std::vector<GLIndexBufferHandle> SubMeshIndices;
    std::vector<int> ElementCount;

    for(auto &Index : Data.Indices) {
        GLIndexBufferHandle IndicesHandle = RHI->CreateIndexBuffer(
            Index.size()*sizeof(unsigned int),
            RNW1,
            Index.data()
            );
        SubMeshIndices.push_back(IndicesHandle);
        ElementCount.push_back((int)Index.size());
    }

    std::vector<GLVertexAssembleHandle> VertexAssembles;
    for(size_t i=0; i<SubMeshIndices.size(); ++i) {
        GLVertexAssembleHandle VertexAssemble = RHI->CreateVertexAssemble();
        RHI->BindVertexBufferToVertexAssemble(VertexAssemble, VertexBuffer, sizeof(StaticMeshVertex));
        RHI->SetVertexAssembleAttribute(VertexAssemble, AttribGetter<StaticMeshVertex>().GetAttrib());
        RHI->BindIndexBufferToVertexAssemble(VertexAssemble, SubMeshIndices[i]);
        VertexAssembles.push_back(VertexAssemble);
    }


    Renderable* Model = new Renderable(VertexBuffer, std::move(SubMeshIndices), std::move(VertexAssembles), std::move(ElementCount));

    auto RenderableMaterials = FileJson["Materials"].array_items();
    size_t i = 0;
    for( ;i<RenderableMaterials.size() && i < Model->GetSubMeshCount(); ++i) {
        std::string MaterialName = RenderableMaterials[i].string_value();
        auto IterMaterial = Materials.find(MaterialName);
        if(IterMaterial != Materials.end()) {
            Model->SetMaterial(i, new RuntimeMaterial(IterMaterial->second));
        } else {
            Model->SetMaterial(i, new RuntimeMaterial(Materials.find("Default")->second));
        }
    }

    for(; i < Model->GetSubMeshCount(); ++i) {
        Model->SetMaterial(i, new RuntimeMaterial(Materials.find("Default")->second));
    }

    if(Iter != Renderables.end()) {
       Renderable* Old = Iter->second;
        if(Old) {
            DestroyRenderableAsset(RHI, Old);
        }
        delete Old;
    }

    Renderables[Name] = Model;
    return true;
}

Texture* AssetSystem::GetTexture(const std::string& Name) const {
    UNI_GET_FUNC(Textures, Name)
}

Material* AssetSystem::GetMaterial(const std::string& Name) const {
    UNI_GET_FUNC(Materials, Name)
}

Renderable* AssetSystem::GetRenderable(const std::string& Name) const {
    UNI_GET_FUNC(Renderables, Name)
}


