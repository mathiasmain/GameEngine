#pragma once

#include "S.h"
#include "V.h"
#include <glm/glm.hpp>
#include <iostream>
#include <cassert>

#define DEBUG false



std::expected<int, std::string> Divide(int a, int b)
{
    if (b == 0)
        return std::unexpected("Divide by zero is impossible... At least for mathmaticians.");

    return (a / b);
}

int main()
{
    bool enableValidationLayers = false;

#if DEBUG false
    enableValidationLayers = true;
#endif
    // auto Result = Divide(10, 0)
    //                   .and_then([](int Result)
    //                             { return Divide(Result, 2); })
    //                   .or_else([](const std::string &error)
    //                            {
    //     std::println("Error: {}", error);
    //     return std::expected<int,std::string>{0}; });
    // std::println("Result = {}", Result.value());

    S sdl(720, 1280, "Vulkan");

    V vulkan("Vulkan", enableValidationLayers, sdl.window);
    std::println("[NXLL INFO]: Initial library configuration was complete succesfuly.");

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    // ----- Main loop -----

    bool running = true;
    SDL_Event event;
    while (running)
    {
        // Poll for events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q)
                {
                    printf("Quitting...\n");
                    running = false;
                }
                break;
            default:
                continue;
            }
        }
    }
    if (enableValidationLayers)
    {
        vulkan.cleanupDebugUtilsMessengerEXT();
    }
    return 0;
}