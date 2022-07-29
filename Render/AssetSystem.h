#pragma once
#include <filesystem>
#include <unordered_map>
#include <assimp/scene.h>

#include "../Material/Material.h"
#include "../Mesh/MeshData.h"
#include "../Mesh/Renderable.h"
#include "../RHI/GLHandleDef.h"
#include "../RHI/OpenGLRHI.h"

class AssetSystem {

    std::unordered_map<std::string, GLShaderHandle> Shaders;
    std::unordered_map<std::string, GLShaderProgramHandle> ShaderPrograms;
    std::unordered_map<std::string, Texture*> Textures;
    std::unordered_map<std::string, Material*> Materials;
    std::unordered_map<std::string, Renderable*> Renderables;

    void ParseParamFromJson(MaterialParam& PushTo, const json11::Json& JsonObject);

    void ProcessMeshData(MeshData& ToData, const aiScene* Scene, aiNode* Node);

    void ProcessMesh(MeshData& ToData, aiMesh* Mesh);

    void DestroyRenderableAsset(OpenGLRHI *RHI, Renderable* Model);
    
public:

    AssetSystem();
    ~AssetSystem();

    std::string LoadTextFileToStr(const std::string& FilePath);
    
    void ClearAll(OpenGLRHI* RHi);

    void ClearShader(OpenGLRHI* RHI);

    void ClearAllShaderProgram(OpenGLRHI* RHI);

    void ClearAllRenderables(OpenGLRHI* RHI);

    void ClearAllMaterials(OpenGLRHI* RHI);

    void ClearAllTextures(OpenGLRHI* RHI);
    
    
    bool LoadVertexShader(OpenGLRHI* RHI, const std::string& Name, const std::string& Path, bool ExistOverride = true);

    bool LoadFragmentShader(OpenGLRHI* RHI, const std::string& Name, const std::string& Path, bool ExistOverride = true);
    
    bool LoadShaderProgram(OpenGLRHI* RHI, const std::string& Path, bool ExistOverride = true);

    bool LoadTexture(OpenGLRHI* RHI, const std::string& Name, const std::string& Path, bool SRGB = false, bool ExistOverride = true);

    bool LoadTexture(OpenGLRHI* RHI, const std::string& Path, bool ExistOverride = true);
    
    bool LoadTextureHDR(OpenGLRHI* RHI, const std::string& Name, const std::string& Path,  bool ExistOverride = true);

    bool LoadHDRCube(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir, bool ExistOverride = true);

    bool LoadCube(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir, bool SRGB = false, bool ExistOverride = true);
    
    bool LoadMaterial(const std::string& Path, bool ExistOverride = true);

    bool LoadRenderable(OpenGLRHI* RHI, const std::string& Path, const std::filesystem::path& BaseDir, bool ExistOverride = true);

    Texture* GetTexture(const std::string& Name) const;
    
    Material* GetMaterial(const std::string& Name) const;

    Renderable* GetRenderable(const std::string& Name) const;
    
};
