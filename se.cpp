#include <cstdio>
#include <iostream>
#include <cstdarg>
#include <cstdlib>
#include <SDL.h>

using namespace std;

const int MAX_PATH_LEN = 100;
const string IMAGE_PATH = "images";

//Screen dimension constants 
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

enum Surfaces {
	SUR_START,
	SUR_STEPPE,
	SUR_PLANE,
	SUR_TOTAL,
};

const string Images[] = {"start.bmp", "steppe.bmp", "kws1.bmp"};

void
error(string msg, ...) {
	va_list arg;
	va_start(arg, msg);
	vfprintf(stderr, msg.c_str(), arg);
	exit(EXIT_FAILURE);
}

SDL_Surface *
load_media(string src, SDL_Surface *screen) {
	SDL_Surface *img, *opt_img;
	string path = "";
	path = IMAGE_PATH + "/" + src;
	img = SDL_LoadBMP(path.c_str());
	if (img == NULL) {
		error("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	opt_img = SDL_ConvertSurface(img, screen->format, 0);
	if (opt_img == NULL) {
		error("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	//Get rid of unoptimalized surface
	SDL_FreeSurface(img);
	return opt_img;
}

void
free_media(SDL_Surface *img) {
	SDL_FreeSurface(img);
}


void
load_media_all(SDL_Surface *surfaces[], SDL_Surface *screen) {
	for (int i = 0; i < SUR_TOTAL; i++) {
		surfaces[i] = load_media(Images[i], screen);
	}
}

void
free_media_all(SDL_Surface *surfaces[]) {
	for (int i = 0; i < SUR_TOTAL; i++) {
		free_media(surfaces[i]);
	}
}

int
main() {
	SDL_Surface *screen, *surfaces[SUR_TOTAL];
	SDL_Window *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("Steppe Ride", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		 error("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	} 

	screen = SDL_GetWindowSurface(window);
	load_media_all(surfaces, screen);

	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = SCREEN_WIDTH;
	screen_rect.h = SCREEN_HEIGHT;

	SDL_BlitScaled(surfaces[SUR_START], NULL, screen, &screen_rect);

	SDL_UpdateWindowSurface(window);

	SDL_Event e;
	SDL_Surface *cur_surface;
	bool quit = false;
	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						cur_surface = surfaces[SUR_PLANE];
					break;
				}
			}
		}

		SDL_BlitSurface(cur_surface, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);
	}

	free_media_all(surfaces);

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
