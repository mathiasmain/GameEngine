#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

class S
{
    public:
        SDL_Window* window = nullptr;
        uint16_t height = 720;
        uint16_t lenght = 1280;

        S(uint16_t height, uint16_t lenght, std::string title);
        ~S();
};