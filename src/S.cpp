#include "S.h"
#include <iostream>

S::S(uint16_t height, uint16_t lenght, std::string title)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Quit();
        std::cerr << "[SDL ERROR]: SDL could not be initialized because:\n" << SDL_GetError() << "\n";
    }

    this->height = height;
    this->lenght = lenght;
	this->window = SDL_CreateWindow(title.c_str(), (int32_t)this->lenght, (int32_t)this->height,
                    SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    if (!this->window) {
		SDL_Quit();
        std::cout << "[SDL ERROR]: Window creation failed:\n" << SDL_GetError() << "\n";
    }

    SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	std::cout << "[SDL INFO]: SDL window created\n";

	
	// Make the window visible
	SDL_ShowWindow(this->window);
    
}

S::~S()
{
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    std::cout << "[SDL INFO]: Ending SDL stuff.\n";
}