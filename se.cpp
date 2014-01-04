#include <cstdio>
#include <iostream>
#include <cstdarg>
#include <cstdlib>
#include <SDL.h>

using namespace std;

const int MAX_PATH_LEN = 100;
const string IMAGE_PATH = "images";

//Screen dimension constants 
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum Surfaces {
	SUR_START,
	SUR_STEPPE,
	SUR_TOTAL,
};

const string Images[] = {"start.bmp", "steppe.bmp"};

void
error(string msg, ...) {
	va_list arg;
	va_start(arg, msg);
	vfprintf(stderr, msg.c_str(), arg);
	exit(EXIT_FAILURE);
}

SDL_Surface *
load_media(string src) {
	SDL_Surface *img;
	string path = "";
	path = IMAGE_PATH + "/" + src;
	img = SDL_LoadBMP(path.c_str());
	if (img == NULL) {
		error("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	return img;
}

void
free_media(SDL_Surface *img) {
	SDL_FreeSurface(img);
}


void
load_media_all(SDL_Surface *surfaces[]) {
	for (int i = 0; i < SUR_TOTAL; i++) {
		surfaces[i] = load_media(Images[i]);
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

	load_media_all(surfaces);

	SDL_BlitSurface(surfaces[SUR_START], NULL, screen, NULL);

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
						cur_surface = surfaces[SUR_STEPPE];
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
