#define SDL_MAIN_HANDLED 1
#include "Application.h"
#include <nfd.hpp>

int main( int argc, char* argv[] )
{
	NFD_Init();

	Application app;
	app.Run();
	app.ShutDown();

	return 0;
}