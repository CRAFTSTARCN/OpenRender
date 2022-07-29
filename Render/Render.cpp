#include "Render.h"

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <json11/json11.hpp>
#include <sstream>

#define STRICT_MODE
#define DEBUG

#define CheckEmpty(Str) if((Str).empty()) { Terminate(); assert(false); } 

void Render::InitShaders(json11::Json JsonFile) {
    for(auto &Shader : JsonFile["VertexShaders"].array_items()) {
        std::filesystem::path Path = ContentFolder / Shader["Path"].string_value();
        bool Res = AssetManager.LoadVertexShader(RHI, Shader["Name"].string_value(), Path.generic_string());
        
#ifdef STRICT_MODE
        if(!Res) {
            Terminate();
            assert(false);
        }
#endif
        
    }

    for(auto &Shader : JsonFile["FragmentShaders"].array_items()) {
        std::filesystem::path Path = ContentFolder / Shader["Path"].string_value();
        bool Res = AssetManager.LoadFragmentShader(RHI, Shader["Name"].string_value(), Path.generic_string());

#ifdef STRICT_MODE
        if(!Res) {
            Terminate();
            assert(false);
        }
#endif
    }
}

void Render::InitShaderPrograms(json11::Json JsonFile) {
    for(auto &ShaderProgram : JsonFile["ShaderPrograms"].array_items()) {
        std::filesystem::path Path = ContentFolder / ShaderProgram.string_value();
        AssetManager.LoadShaderProgram(RHI, Path.generic_string());
    }
}

void Render::InitTextures(json11::Json JsonFile) {
    for(auto &Texture : JsonFile["Textures"].array_items()) {
        std::filesystem::path Path = ContentFolder / Texture["Path"].string_value();
        bool IsSRGBTexture = Texture["sRGB"].bool_value(); 
        AssetManager.LoadTexture(RHI,  Texture["Name"].string_value(), Path.generic_string(), IsSRGBTexture);
    }

    for(auto &TextureHDR : JsonFile["TextureHDR"].array_items()) {
        std::filesystem::path Path = ContentFolder / TextureHDR["Path"].string_value();
        AssetManager.LoadTextureHDR(RHI,  TextureHDR["Name"].string_value(), Path.generic_string());
    }

    for(auto &CubeTex : JsonFile["CubeTex"].array_items()) {
        std::filesystem::path Path = ContentFolder / CubeTex.string_value();
        AssetManager.LoadCube(RHI, Path.generic_string(), ContentFolder.c_str());
    }

    for(auto &CubeTexHDR : JsonFile["CubeTexHDR"].array_items()) {
        std::filesystem::path Path = ContentFolder / CubeTexHDR.string_value();
        AssetManager.LoadHDRCube(RHI, Path.generic_string(), ContentFolder);
    }
    
}

void Render::InitMaterials(json11::Json JsonFile) {
    for(auto &Material : JsonFile["Materials"].array_items()) {
        std::filesystem::path Path = ContentFolder / Material.string_value();
        AssetManager.LoadMaterial(Path.generic_string());
    }
}

void Render::InitRenderables(json11::Json JsonFile) {
    for(auto &Renderable : JsonFile["Renderables"].array_items()) {
        std::filesystem::path Path = ContentFolder / Renderable.string_value();
        AssetManager.LoadRenderable(RHI, Path.generic_string(), ContentFolder);
    }
}

void Render::InitScene(json11::Json Scene) {
    InitRenderableInstances(Scene);
    InitLight(Scene);
    InitCamera(Scene);
}

void Render::AddOneInstanceToScene(json11::Json Json) {
    std::string Name = Json["Name"].string_value();
    CheckEmpty(Name)
    Renderable* BaseRenderabe = AssetManager.GetRenderable(Name);
    if(!BaseRenderabe) {
        return;
    }

    RenderableInstance* Instance = new RenderableInstance(BaseRenderabe);
    json11::Json Transform = Json["Transform"];

    glm::vec3 T = {0.0,0.0,0.0};
    glm::vec3 R = {0.0,0.0,0.0};
    glm::vec3 S = {1,1,1};
    
    json11::Json Translate = Transform["Translate"];
    T.x = (float)Translate["x"].number_value();
    T.y = (float)Translate["y"].number_value();
    T.z = (float)Translate["z"].number_value();
    json11::Json Rotation = Transform["Rotation"];
    R.x = (float)Rotation["x"].number_value();
    R.y = (float)Rotation["y"].number_value();
    R.z = (float)Rotation["z"].number_value();
    json11::Json Scale = Transform["Scale"];
    S.x = (float)Scale["x"].number_value();
    S.y = (float)Scale["y"].number_value();
    S.z = (float)Scale["z"].number_value();

    Instance->SetTranslate(T);
    Instance->SetOrientation(R);
    Instance->SetScale(S);

    int i = 0;
    for(auto &Mat : Json["Materials"].array_items()) {
        Material *BaseMaterial = AssetManager.GetMaterial(Mat.string_value());
        Instance->SetMaterial(i, new RuntimeMaterial(BaseMaterial));
    }

    Instances.push_back(Instance);
}

void Render::InitRenderableInstances(json11::Json Scene) {
    for(auto &Instance : Scene["Instances"].array_items()) {
        AddOneInstanceToScene(Instance);
    }
}

void Render::InitLight(json11::Json Scene) {
    json11::Json LightJson = Scene["Light"];
    json11::Json LightColor = LightJson["Color"];
    json11::Json LightDir = LightJson["Direction"];

    SceneLight.SetColor({
        (float)LightColor["x"].number_value(),(float)LightColor["y"].number_value(),(float)LightColor["z"].number_value(),1.0f
    });
    SceneLight.SetDirection({
        (float)LightDir["x"].number_value(),(float)LightDir["y"].number_value(),(float)LightDir["z"].number_value(),0.0f
    });
}

void Render::InitCamera(json11::Json Scene) {
    json11::Json CameraJson = Scene["Camera"];
    json11::Json InitPosition = CameraJson["InitPosition"];

    SceneCamera.Position = {(float)InitPosition["x"].number_value(),(float)InitPosition["y"].number_value(),(float)InitPosition["z"].number_value()};

    float Near = (float)CameraJson["Near"].number_value();
    float Far = (float)CameraJson["Far"].number_value();
    float Fov = (float)CameraJson["Fov"].number_value();

    SceneCamera.Near = Near;
    SceneCamera.Far = std::max(Near, Far);
    SceneCamera.FOV = std::min(90.0f, std::abs(Fov));
    
}

void Render::InitSkyBoxIBL() {
    Material* BaseSkyBoxMaterial =  AssetManager.GetMaterial("Skybox");
    IBLRadiance = AssetManager.GetTexture("RadianceMap");
    IBLIrradiance = AssetManager.GetTexture("IrradianceMap");
    BRDFLut = AssetManager.GetTexture("BRDFLut");

    if(!BaseSkyBoxMaterial || !IBLIrradiance || !IBLIrradiance || !BRDFLut) {
        assert(false);
    }

    SkyBoxMaterial = new RuntimeMaterial(BaseSkyBoxMaterial);

    float SkyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    SkyBoxRenderable = RHI->CreateVertexAssemble();
    SkyboxVertexBuffer = RHI->CreateVertexBuffer(6ll * 6 * 3 * sizeof(float), RNW1, SkyboxVertices);
    RHI->BindVertexBufferToVertexAssemble(SkyBoxRenderable, SkyboxVertexBuffer, 3*sizeof(float) );
    RHI->SetVertexAssembleAttribute(SkyBoxRenderable, {
        {
            0, 3, 0, 3*sizeof(float)
        }});
}

void Render::SolveMainCameraView() {
    SceneCamera.TestUpdate();
    CameraView = SceneCamera.GetViewMatrix();
    Projection = SceneCamera.GetProjectionMatrix();
}

void Render::SetUpLight() {
    RHI->SetUniformBufferData(LightUniformBuffer, 0, (int64_t)SceneLight.GetUniformBlock()->UniformSize, SceneLight.GetUniformBlock()->Data);
}

void Render::FromRendData() {

    std::vector<RendInfo>& Info = RenderInfoCache;
    Info.clear();
    
    for(auto Instance : Instances) {
        bool InstanceUseElement = Instance->IsUseIndex();
        glm::vec4 ViewCoord = CameraView * glm::vec4{Instance->GetTransform()->Translate, 1.0f};
#ifdef DEBUG
        glm::vec4 ProjectionCoord = Projection * ViewCoord;
#endif
        
        for(int i=0; i<Instance->GetSubMeshCount(); ++i) {
            GLVertexAssembleHandle SubMesh = Instance->GetSubMesh(i);
            const RuntimeMaterial* RT = Instance->GetMaterial(i);
            int Offset = InstanceUseElement ? 0 : Instance->GetIndexBuffer(i);
           
            Info.push_back( {
                    RT->GetMaterial(),
                    RT->GetParamPtr(),
                    Instance->GetTransform(),
                    DrawMod::Triangle,
                    Instance->GetElementCount(i),
                    Offset,
                    ViewCoord.z,
                    SubMesh,
                    InstanceUseElement,
                    RT->MaterialParamChanged()
                });
        }
    }

    std::sort(Info.begin(), Info.end(), [](const RendInfo& L, const RendInfo R) -> bool
    {
        if(abs(L.ViewCoordZ - R.ViewCoordZ) > 0.005f) {
            return L.ViewCoordZ < R.ViewCoordZ;
        }
        
        if(L.UseMaterial != R.UseMaterial) {
            return L.UseMaterial->GetShaderHandle() < R.UseMaterial->GetShaderHandle();
        }

        if(!L.ParamChanged && R.ParamChanged) {
            return true;
        }

        return false;
    });
}

glm::mat4 Render::GetModelMatrix(const Transform* FromTransform) {
    glm::mat4 M2W(1.0f);
    
    M2W = glm::scale(M2W, FromTransform->Scale);
    M2W = glm::mat4_cast(FromTransform->Orientation) * M2W;
    
    M2W[3][0] = FromTransform->Translate[0];
    M2W[3][1] = FromTransform->Translate[1];
    M2W[3][2] = FromTransform->Translate[2];

    return M2W;
}

void Render::RendOneObject(const RendInfo& Info, bool RestUniform, bool ResetShader) {
    GLShaderHandle Shader = Info.UseMaterial->GetShaderHandle();

    if(RestUniform) {
        if(ResetShader) {
            RHI->UseShaderProgram(Shader);
        }
        const auto *ParamTable = Info.Params->GetParamTable();
        const auto *TexTable = Info.Params->GetTextureTable();
        for(auto Iter = ParamTable->begin(); Iter != ParamTable->end(); ++Iter) {
            RHI->SetUniform(Shader, Iter->first, Iter->second);
        }

        int TexUnit = 0;
        for(auto Iter=TexTable->begin(); Iter != TexTable->end(); ++Iter) {
            RHI->SetUniformTexture(Shader, Iter->first, Iter->second->GetTextureHandle(), TexUnit);
            ++TexUnit;
        }

        if(ResetShader) {
            if(Info.UseMaterial->IsUseIBL()) {
                RHI->SetUniformCubeTexture(Shader, "_IBL_Radiance", IBLRadiance->GetTextureHandle(), TexUnit++);
                RHI->SetUniformCubeTexture(Shader, "_IBL_Irradiance", IBLIrradiance->GetTextureHandle(), TexUnit++);
                RHI->SetUniformTexture(Shader, "_BRDFLut", BRDFLut->GetTextureHandle(), TexUnit);
            }
        }

        RHI->SetUniformBlock(Shader, "_TransfromMatrices", 0);
        RHI->SetUniformBlock(Shader, "_SceneLight", 1);
        Uniform CameraPos(UniformTypeVector4, true);
        CameraPos.Data = &SceneCamera.Position;
        RHI->SetUniform(Shader, "_CameraPos", CameraPos);
    }
 
    glm::mat4 M2W = GetModelMatrix(Info.ObjectTransform);
    glm::mat4 MVP = Projection * CameraView * M2W;
    glm::mat4 N2W = glm::transpose(glm::inverse(M2W));
    RHI->SetUniformBufferData(TransformMatricesBlock, 0, 16*sizeof(float), glm::value_ptr(MVP));
    RHI->SetUniformBufferData(TransformMatricesBlock, 16*sizeof(float), 16*sizeof(float), glm::value_ptr(M2W));
    RHI->SetUniformBufferData(TransformMatricesBlock, 32*sizeof(float), 16*sizeof(float), glm::value_ptr(N2W));
    
    RHI->DrawVertexAssemble({
        Info.VertexAssemble,
        Info.ObjectDrawMode,
        Info.Offset,
        Info.Count,
        Info.UseElement
    });
}

void Render::RenderingSkyBox() {
    GLShaderHandle Shader =SkyBoxMaterial->GetMaterial()->GetShaderHandle();
    RHI->UseShaderProgram(Shader);

    const auto *ParamTable = SkyBoxMaterial->GetParamPtr()->GetParamTable();
    const auto *TexTable = SkyBoxMaterial->GetParamPtr()->GetTextureTable();

  
    for(auto Iter = ParamTable->begin(); Iter != ParamTable->end(); ++Iter) {
        RHI->SetUniform(Shader, Iter->first, Iter->second);
    }

    int TexUnit = 0;
    for(auto Iter=TexTable->begin(); Iter != TexTable->end(); ++Iter) {
        RHI->SetUniformCubeTexture(Shader, Iter->first, Iter->second->GetTextureHandle(), TexUnit);
        ++TexUnit;
    }

    glm::mat4 SkyBoxView = glm::mat4(glm::mat3(CameraView));
    glm::mat4 SkyBoxMVP = Projection * SkyBoxView;

    Uniform MVPUniform(UniformTypeMatrix4_4, true);
    MVPUniform.Data = &SkyBoxMVP;
    RHI->SetUniform(Shader, "_MVP", MVPUniform);
    RHI->SetDepthFunction(LessEqual);
    RHI->DrawVertexAssemble({SkyBoxRenderable, DrawMod::Triangle, 0, 36, false});
    RHI->SetDepthFunction(Less);
}

void Render::RendFrame() {
    SetUpLight();
    SolveMainCameraView();
    FromRendData();

    RHI->ClearBuffers();

    if(!RenderInfoCache.empty()) {
        RendOneObject(RenderInfoCache[0], true, true);
        for(size_t i=1; i<RenderInfoCache.size(); ++i) {
            bool ResetUniform = true, ResetShader = true;
            if(RenderInfoCache[i].UseMaterial == RenderInfoCache[i-1].UseMaterial) {
                ResetShader = false;
                if(!RenderInfoCache[i].ParamChanged) {
                    ResetUniform = false;
                }
            }

            RendOneObject(RenderInfoCache[i], ResetUniform, ResetShader);
        }
    }

    RenderingSkyBox();
    RHI->Present();
    
}

Render::Render() :
    SceneLight(),
    SceneCamera({0,0,-1},{0, -90, 0}, 1.0f),
    CameraView{},
    Projection{},
    RHI(new OpenGLRHI),
    LightUniformBuffer(0),
    TransformMatricesBlock(0),
    IBLRadiance(nullptr),
    IBLIrradiance(nullptr),
    BRDFLut(nullptr),
    SkyBoxMaterial(nullptr),
    Window(nullptr)
    
{
    
}

void Render::PreInit() {
    std::string ConfigContent = AssetManager.LoadTextFileToStr("./Info.ini");
    std::istringstream ConfigStream(ConfigContent);
    int Wd, Ht;
    std::string Titile;
    ConfigStream>>Wd>>Ht>>Titile;

    RHI->PresetTitle(Titile);
    RHI->PresetWidthAndHeight(Wd, Ht);
    SceneCamera.AspectRatio = ((float)Wd) / ((float)Ht);
    RHI->PreInit();

}

void Render::Initialize(const std::string& ConfigFilePath) {
    
    RHI->Init();
    Window = RHI->GetRenderWindow();
    LightUniformBuffer = RHI->CreateUniformBuffer((long long)SceneLight.GetUniformBlock()->UniformSize);
    TransformMatricesBlock = RHI->CreateUniformBuffer(3ll * 16ll * sizeof(float));

    RHI->BindUniformBufferToPoint(TransformMatricesBlock, 0);
    RHI->BindUniformBufferToPoint(LightUniformBuffer, 1);

    std::string Str = AssetManager.LoadTextFileToStr(ConfigFilePath);
    std::string Err;
    json11::Json JsonFile = json11::Json::parse(Str, Err);
    if(JsonFile.is_null()) {
        Terminate();
        assert(false);
    }

    ContentFolder = JsonFile["ContentFolder"].string_value();
    CheckEmpty(ContentFolder)

    InitShaders(JsonFile);
    InitShaderPrograms(JsonFile);
    InitTextures(JsonFile);
    InitMaterials(JsonFile);
    InitRenderables(JsonFile);

    InitSkyBoxIBL();
    
    InitScene(JsonFile["Scene"]);
}

void Render::Run() {
    while(!Window->ShouldClose()) {
        RendFrame();
        Window->SubmitIOMessage();
    }
}

void Render::Terminate() {
    for(auto Instance : Instances) {
        delete Instance;
    }
    delete SkyBoxMaterial;
    RHI->DeleteVertexAssemble(SkyBoxRenderable);
    RHI->DeleteBuffer(SkyboxVertexBuffer);
    AssetManager.ClearAll(RHI);
    RHI->Terminate();
    delete RHI; 
}
