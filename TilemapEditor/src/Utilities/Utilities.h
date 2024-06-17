#pragma once
#include <SDL.h>
#include <memory>

struct Util
{
	struct SDLDestroyer
	{
		void operator()( SDL_Window* window ) const { SDL_DestroyWindow( window ); }
		void operator()( SDL_Renderer* renderer ) const { SDL_DestroyRenderer( renderer ); }
		void operator()( SDL_Texture* texture ) const { SDL_DestroyTexture( texture ); }
	};

	static float Lerp( float a, float b, float t ) { return a + t * ( b - a ); }
};

typedef std::unique_ptr<SDL_Window, Util::SDLDestroyer> Window;
typedef std::unique_ptr<SDL_Renderer, Util::SDLDestroyer> Renderer;
typedef std::unique_ptr<SDL_Texture, Util::SDLDestroyer> Texture;