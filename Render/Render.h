#pragma once
#include <json11/json11.hpp>

#include "AssetSystem.h"
#include "Camera.h"
#include "Light.h"
#include "RendInfo.h"
#include "../Mesh/RenderableInstance.h"
#include "../RHI/GLHandleDef.h"
#include "../RHI/OpenGLRHI.h"

class Render {
    
    std::vector<RenderableInstance*> Instances;
    DirectionalLight SceneLight;
    Camera SceneCamera;

    glm::mat4 CameraView;
    glm::mat4 Projection;

    OpenGLRHI* RHI;
    GLUniformBufferHandle LightUniformBuffer;
    GLUniformBufferHandle TransformMatricesBlock;

    GLVertexAssembleHandle SkyBoxRenderable;
    GLVertexAssembleHandle SkyboxVertexBuffer;
    
    Texture *IBLRadiance;
    Texture *IBLIrradiance;
    Texture *BRDFLut;
    
    RuntimeMaterial *SkyBoxMaterial;

    AssetSystem AssetManager;
    std::filesystem::path ContentFolder;

    RenderWindow* Window;

    std::vector<RendInfo> RenderInfoCache;

    void InitShaders(json11::Json JsonFile);

    void InitShaderPrograms(json11::Json JsonFile);

    void InitTextures(json11::Json JsonFile);

    void InitMaterials(json11::Json JsonFile);

    void InitRenderables(json11::Json JsonFile);

    void InitScene(json11::Json Scene);

    void AddOneInstanceToScene(json11::Json Json);

    void InitRenderableInstances(json11::Json Scene);

    void InitLight(json11::Json Scene);

    void InitCamera(json11::Json Scene);

    void InitSkyBoxIBL();

    void SolveMainCameraView();

    void SetUpLight();

    void FromRendData();

    glm::mat4 GetModelMatrix(const Transform* FromTransform);

    void RendOneObject(const RendInfo& Info, bool RestUniform, bool ResetShader);

    void RenderingSkyBox();
    
    void RendFrame();


public:

    Render();

    void PreInit();

    void Initialize(const std::string& ConfigFilePath);
    
    void Run();

    void Terminate();
    
};
