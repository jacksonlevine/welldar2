#pragma once

#define RENDERER_IMP
#define MAINWIZARD_IMP
#include "wegot/renderer.hpp"


void main()
{

Renderer renderer;

while(!glfwWindowShouldClose(renderer.window))
{
    renderer.render();
}

}