#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "assert.h"

class Window
{

public:
    GLFWwindow *m_GLFW_Window = nullptr;

    std::string m_Title;
    uint16_t m_Width = 0;
    uint16_t m_Height = 0;

    Window(uint16_t hight, uint16_t width, std::string title);
    ~Window();

    // void RenderUI(std::string imgui_window_name);

    void OnUpdate();
};
