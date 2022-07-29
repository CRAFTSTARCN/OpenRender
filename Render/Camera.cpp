#include "Camera.h"

#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Camera::TestUpdate() {
    PitchYawRoll.y += 0.1f;
}

Camera::Camera(glm::vec3 CamPosition, glm::vec3 CamRotation, float CamAspectRatio, float CamNear, float CamFar,
               float CamFOV):
    Position(CamPosition), PitchYawRoll(CamRotation),
    AspectRatio(CamAspectRatio), Near(CamNear), Far(CamFar), FOV(CamFOV)

{
    
}

glm::mat4 Camera::GetViewMatrix() const {
    glm::vec3 LookDirection;
    LookDirection.y = sin(glm::radians(PitchYawRoll.x));
    LookDirection.x = cos(glm::radians(PitchYawRoll.x)) * cos(glm::radians(PitchYawRoll.y));
    LookDirection.z = cos(glm::radians(PitchYawRoll.x)) * sin(glm::radians(PitchYawRoll.y));

    LookDirection = glm::normalize(LookDirection);
    glm::quat Up = glm::rotate({0,0,1,0}, PitchYawRoll.z, LookDirection);
    return glm::lookAt(Position, Position+LookDirection, {Up.x, Up.y, Up.z});
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(FOV), AspectRatio, Near, Far);
}
