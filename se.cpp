#include <cstdio>
#include <iostream>
#include <cstdarg>
#include <cstdlib>
#include <SDL.h>

//Game Scale: 1px = 0,21559m
//River Length: 525km = 525000m = 2435177px
//KWS1: Rozpietosc: 9,04m Dlugosc: 7,33m(34px) Wysokosc: 2,7m
using namespace std;

const int MAX_PATH_LEN = 100;
const string IMAGE_PATH = "images";

//Screen dimension constants 
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

const int FRAMES_PER_SECOND = 20;

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
	Uint32 color_key = SDL_MapRGB(opt_img->format, 0, 0xFF, 0xFF);
	SDL_SetColorKey(opt_img, SDL_TRUE, color_key);
	return opt_img;
}

void
applay_surface(int x, int y, SDL_Surface *source, SDL_Surface *dest) {
    //Temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Get the offsets
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface(source, NULL, dest, &offset);
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
get_ticks(int start_ticks) {
	return SDL_GetTicks() - start_ticks;
}

void
draw_map(SDL_Surface *screen) {
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
}

void
show_plane(SDL_Surface *plane, SDL_Surface *screen) {
	applay_surface((SCREEN_WIDTH/2) - (plane->w/2), SCREEN_HEIGHT-plane->h, plane, screen);
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

	//waiting for game to start
	SDL_Event e;
	bool quit = false;

	while (quit == false) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						draw_map(screen);
						quit = true;
					break;
				}
			}
		}
	}
	quit = false;

	//Keep track of the current frame
	int frame = 0;

	int start_ticks;

	while (quit == false) {
		//Start teh frame timer
		start_ticks = SDL_GetTicks();

		show_plane(surfaces[SUR_PLANE], screen);

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				}
			}
		}
		SDL_UpdateWindowSurface(window);
		frame++;

		if (get_ticks(start_ticks) < 1000/FRAMES_PER_SECOND) {
			SDL_Delay((1000/FRAMES_PER_SECOND) - get_ticks(start_ticks));
		}
	}

	free_media_all(surfaces);

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
