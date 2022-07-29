#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>


class Camera {

public:

    glm::vec3 Position;
    glm::vec3 PitchYawRoll;
    
    float AspectRatio;
    float Near;
    float Far;
    float FOV;

    Camera(glm::vec3 CamPosition, glm::vec3 CamRotation, float CamAspectRatio,  float CamNear = 1.0f, float CamFar = 3000.0f, float CamFOV = 75.0f);

    glm::mat4 GetViewMatrix() const;
    
    glm::mat4 GetProjectionMatrix() const;
    void TestUpdate();
};
