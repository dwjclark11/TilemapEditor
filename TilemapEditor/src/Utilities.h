#pragma once
#include <SDL.h>

static struct Util
{
	struct SDLDestroyer
	{
		void operator()(SDL_Window* window)		const { SDL_DestroyWindow(window); }
		void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
		void operator()(SDL_Texture* texture)	const { SDL_DestroyTexture(texture); }
	};
};