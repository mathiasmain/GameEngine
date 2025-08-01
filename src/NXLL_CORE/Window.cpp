#include "Window.h"


// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl3.cpp

Window::Window(uint16_t height, uint16_t width, std::string title)
{
    m_Height = height;
    m_Width = width;
    m_Title = title;

    assert(glfwInit(),"[GLFW_ERROR]: The application has failed to initialize GLFW.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_GLFW_Window = glfwCreateWindow(m_Width,m_Height,m_Title.c_str(),nullptr,nullptr);

    assert(m_GLFW_Window != nullptr,"[GLFW_ERROR]: The application has failed to create a window.");
}

Window::~Window()
{
    glfwDestroyWindow(m_GLFW_Window);
    glfwTerminate();
}

void Window::OnUpdate()
{
    glfwPollEvents();

}