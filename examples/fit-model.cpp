
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "toys.h"

#include "callback.h"
#include "mesh.h"
#include "loadMesh.h"

#pragma comment(lib, "opengl32")
#pragma comment(lib, "glfw3")
#pragma comment(lib, "glew32s") 

#include "eos_fitting.h"


void sceneRender(GLFWwindow*);
void init();

int main(int argc, char* argv[])
{
    //dlib_fit();
    eos_fit(argc, argv);

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 8);
    GLFWwindow* window = glfwCreateWindow(1440, 900, "Test", 0, 0);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glewInit();
    init();
    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {
        sceneRender(window);
        glfwPollEvents();
    }
}

using namespace glm;
using namespace std;

Program program;
std::vector<Mesh> meshes;

void init()
{
    meshes = loadMesh("C:\\Users\\se\\source\\repos\\output.obj");
}

void sceneRender(GLFWwindow* window)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.5, 0.5, 0.5, 0); // ¹è°æ»ö
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(program.programID);

    GLuint ModelMatLocation = glGetUniformLocation(program.programID, "modelMat");
    glUniformMatrix4fv(ModelMatLocation, 1, 0, value_ptr(mat4(1)));

    GLuint viewMatLocation = glGetUniformLocation(program.programID, "viewMat");
    glUniformMatrix4fv(viewMatLocation, 1, 0, value_ptr(camera.getViewMat()));

    GLuint projMatLocation = glGetUniformLocation(program.programID, "projMat");
    glUniformMatrix4fv(projMatLocation, 1, 0, value_ptr(camera.getProjMat(w, h)));

    for (Mesh& mesh : meshes)
    {
        mesh.render();
    }

    glfwSwapBuffers(window);
}
