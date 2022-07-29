#ifdef _WIN32

#include <Windows.h>
#include "Render/Render.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Render MainRender;
    MainRender.PreInit();
    MainRender.Initialize("./Project.json");
    MainRender.Run();
    MainRender.Terminate();
    return 0;
}

#endif
