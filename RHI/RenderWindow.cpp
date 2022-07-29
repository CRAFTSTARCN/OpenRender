#include "RenderWindow.h"

#include <GLFW/glfw3.h>

RenderWindow::RenderWindow(uint32_t InWidth, uint32_t Height, GLFWwindow* InWindow) :
    Width(InWidth), Height(Height), GLWindow(InWindow)
{
    
}

bool RenderWindow::ShouldClose() const {
    return glfwWindowShouldClose(GLWindow);
}

void RenderWindow::SubmitIOMessage() {
    return glfwPollEvents();
}
