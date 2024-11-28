#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

const float PI = 3.14159265358979f;

struct Camera {
    float theta = 0;
    float phi = 0;
    float distance = 300; 
    float fovy = 80 * PI / 180;

    glm::mat4 getViewMat() {
        glm::vec3 initialCameraPosition = glm::vec3(0, 0, distance);
        glm::mat4 cameraRotationMatrix1 = glm::rotate(phi, glm::vec3(1, 0, 0));
        glm::mat4 cameraRotationMatrix2 = glm::rotate(theta, glm::vec3(0, 1, 0));
        glm::vec3 cameraPosition = cameraRotationMatrix2 * cameraRotationMatrix1 * glm::vec4(initialCameraPosition, 1);

        return glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }
    glm::mat4 getProjMat(int w, int h) {
        return glm::perspective(fovy, w / (float)h, 0.01f, 100000.f);
    }
};
