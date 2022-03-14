#define SDL_MAIN_HANDLED
#define main SDL_main

#include "Application.h"

int main(int argc, char* argv[])
{
	Application app;

	app.Run();

	return 0;
}