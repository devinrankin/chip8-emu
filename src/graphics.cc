#include "graphics.hh"
#include <cstdio>
#include <stdexcept>

Graphics::Graphics() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize SDL: %s", SDL_GetError());
        throw std::runtime_error("Couldn't initialize SDL");
    }

    if(!SDL_CreateWindowAndRenderer("Chip-8 Emulator", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't intialize window and renderer: %s", SDL_GetError());
        throw std::runtime_error("Couldn't initialize window and renderer");
    }
    SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32); 
    if(!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize texture: %s", SDL_GetError());
        throw std::runtime_error("Couldn't initialize texture");
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}
Graphics::~Graphics() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::render(const void* pixels, int pitch) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer); 
    SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Graphics::handle_input(uint8_t* keypad, bool& running) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT) {
            running = false;
            return;
        }
	
        bool is_pressed = (event.type == SDL_EVENT_KEY_DOWN);
        if(event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.key) {
                case SDLK_ESCAPE: running = false; break;

                case SDLK_1: keypad[0x1] = is_pressed; break;
                case SDLK_2: keypad[0x2] = is_pressed; break;
                case SDLK_3: keypad[0x3] = is_pressed; break;
                case SDLK_4: keypad[0xC] = is_pressed; break;
                
                case SDLK_Q: keypad[0x4] = is_pressed; break;
                case SDLK_W: keypad[0x5] = is_pressed; break;
                case SDLK_E: keypad[0x6] = is_pressed; break;
                case SDLK_R: keypad[0xD] = is_pressed; break;
                
                case SDLK_A: keypad[0x7] = is_pressed; break;
                case SDLK_S: keypad[0x8] = is_pressed; break;
                case SDLK_D: keypad[0x9] = is_pressed; break;
                case SDLK_F: keypad[0xE] = is_pressed; break;
                
                case SDLK_Z: keypad[0xA] = is_pressed; break;
                case SDLK_X: keypad[0x0] = is_pressed; break;
                case SDLK_C: keypad[0xB] = is_pressed; break;
                case SDLK_V: keypad[0xF] = is_pressed; break; 
            }
        }
    }
}
