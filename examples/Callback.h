#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include "camera.h"

extern Camera camera;

void cursorPosCallback(GLFWwindow* win, double xpos, double ypos);
void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);

#endif
