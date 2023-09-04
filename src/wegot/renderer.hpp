#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <functional>
#include <map>
#include <vector>

#include "mainwizard.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render();
    void dropCallback(GLFWwindow* window, int count, const char* paths[]);
    GLFWwindow* window;
    static Renderer* instance;
private:
    MainWizard mainwiz;
};

void dcall(GLFWwindow* window, int count, const char* paths[]);

#ifdef RENDERER_IMP
Renderer* Renderer::instance = nullptr;

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

Renderer::Renderer()
{
    this->instance = this;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(1280, 768, "ImGui Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    // Set the drop callback function
    glfwSetDropCallback(this->window, dcall);

    // Make the OpenGL context current
    glfwMakeContextCurrent(this->window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Renderer::dropCallback(GLFWwindow* window, int count, const char* paths[]) {
    if (count > 0) {
        const std::string droppedFilePath = paths[0];

        this->mainwiz.addFile(droppedFilePath);
    }
}

void dcall(GLFWwindow* window, int count, const char* paths[])
{
    Renderer::instance->dropCallback(window, count, paths);
}

void Renderer::render() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->mainwiz.render();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

#endif