#include <SDL3/SDL.h>

class Graphics {
    public:
	Graphics();
	~Graphics();

	void render(const void* pixels, int pitch);
	void handle_input(uint8_t* keypad, bool& running);
    private:
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};
};
