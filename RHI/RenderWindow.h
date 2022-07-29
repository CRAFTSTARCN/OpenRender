#pragma once
#include <cstdint>

struct GLFWwindow;

class RenderWindow {

    uint32_t Width;
    uint32_t Height;
    GLFWwindow* GLWindow;
    
public:

    RenderWindow(uint32_t InWidth, uint32_t Height, GLFWwindow* InWindow);

    bool ShouldClose() const;

    void SubmitIOMessage();
    
};
