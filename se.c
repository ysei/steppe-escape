#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "utils.h"
#include "config.h"
#include "level.h"
#include "se.h"

#include "levels/l1.xpm"

SDL_Surface *
load_media(const char *src, SDL_Surface *screen) {
	SDL_Surface *img, *opt_img;

	img = SDL_LoadBMP(src);
	if (img == NULL) {
		error("Unable to load image %s! SDL Error: %s\n", src, SDL_GetError());
	}
	opt_img = SDL_ConvertSurface(img, screen->format, 0);
	if (opt_img == NULL) {
		error("Unable to optimize image %s! SDL Error: %s\n", src, SDL_GetError());
	}
	//Get rid of unoptimalized surface
	SDL_FreeSurface(img);

	Uint32 color_key = SDL_MapRGB(opt_img->format, 0, 0xFF, 0xFF);
	SDL_SetColorKey(opt_img, SDL_TRUE, color_key);
	return opt_img;
}

void
apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *dest, SDL_Rect *clip) {
    //Temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Get the offsets
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface(source, clip, dest, &offset);
}

void
free_media(SDL_Surface *img) {
	SDL_FreeSurface(img);
}

//Add slash to path if is needed
void
add_slash_to_path(char *new_path, const char *old_path) {
	int path_len;

	strcpy(new_path, old_path);

	path_len = strlen(new_path);

	new_path[path_len++] = '/';
	new_path[path_len] = '\0';
}

void
load_media_all(SDL_Surface *surfaces[], SDL_Surface *levels[], SDL_Surface *screen) {
	char path[MAX_PATH_LEN]; 

	for (int i = 0; i < SUR_TOTAL; i++) {

		add_slash_to_path(path, IMAGE_PATH);
		strcat(path, Images[i]);

		surfaces[i] = load_media(path, screen);
	}

	for (int i = 0; i < LEVEL_TOTAL; i++) {

		add_slash_to_path(path, LEVEL_PATH);

		char l_name[MAX_PATH_LEN];
		sprintf(l_name, "%s%d.bmp", LEVEL_PREFIX, i+1);

		strcat(path, l_name);

		levels[i] = load_media(path, screen);
	}
}

void
free_media_all(SDL_Surface *surfaces[], SDL_Surface *levels[]) {
	for (int i = 0; i < SUR_TOTAL; i++) {
		free_media(surfaces[i]);
	}
	for (int i = 0; i < LEVEL_TOTAL; i++) {
		free_media(levels[i]);
	}
}

void
set_camera(SDL_Rect *camera, Level *level, Plane *plane) {
	static int road = 0;
	road += plane->speed;

	camera->y = level->sur->h - SCREEN_HEIGHT - road;

	if (camera->y < 0) {
		camera->y = 0;
	}
}

void
show_plane(Plane *plane, SDL_Surface *screen) {
	apply_surface(plane->x, plane->true_y, plane->sur, screen, NULL);
}

void
handle_plane_input(Plane *plane, Level *level, SDL_Event *e) {
	if (e->type == SDL_KEYDOWN) {
		switch(e->key.keysym.sym) {
			case SDLK_LEFT:
				plane->x -= PLANE_X_SPEED;
				break;
			case SDLK_RIGHT:
				plane->x += PLANE_X_SPEED;
				break;

			case SDLK_UP:
				plane->speed += PLANE_SPEED_MULTIPLAYER;
				if (plane->speed > PLANE_SPEED_MAX) {
					plane->speed = PLANE_SPEED_MAX;
				}
				break;
			case SDLK_DOWN:
				plane->speed -= PLANE_SPEED_MULTIPLAYER;
				if (plane->speed < PLANE_SPEED_MIN) {
					plane->speed = PLANE_SPEED_MIN;
				}
				break;
		}
	} 
}

void
move_plane(Plane *plane, Level *level) {
	if (plane->x < 0) {
		plane->x = 0;
	} else if (plane->x > level->sur->w - plane->sur->w) {
		plane->x = level->sur->w - plane->sur->w;
	}

	if (plane->level_y < plane->true_y) {
		plane->true_y = plane->level_y;
	}
	plane->level_y -= plane->speed;
}

int
collision_detect(Rect_Vect *a, Rect_Vect *b) {
	return 0;
}

int
land_collision(Plane *plane, Level *level) {
	//check only positions that matters
	Rect_Vect strip;
	strip.tab = NULL;
	for (size_t i = plane->level_y; i <= plane->level_y + plane->sur->w; i++) {
		vect_add(level->boxes.tab[i], &strip);
	}
	vect_print(&strip);

	return collision_detect(&plane->boxes, &strip);
}

int
main() {
	SDL_Surface *screen, *surfaces[SUR_TOTAL], *levels[LEVEL_TOTAL];
	SDL_Window *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow(
			"Steppe Ride", 
			 SDL_WINDOWPOS_UNDEFINED,
			 SDL_WINDOWPOS_UNDEFINED,
			 SCREEN_WIDTH,
			 SCREEN_HEIGHT,
			 SDL_WINDOW_SHOWN);
	if (window == NULL) {
		 error("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	} 

	screen = SDL_GetWindowSurface(window);
	load_media_all(surfaces, levels, screen);

	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = SCREEN_WIDTH;
	screen_rect.h = SCREEN_HEIGHT;

	SDL_BlitScaled(surfaces[SUR_START], NULL, screen, &screen_rect);

	SDL_UpdateWindowSurface(window);

	//waiting for game to start
	SDL_Event e;
	int quit = 0;

	while (quit == 0) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = 1;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						quit = 1;
					break;
				}
			}
		}
	}
	quit = 0;

	int start_ticks;

	Level level;
	level.sur = levels[LEVEL_1];

	load_level(&level, l1_xpm, LEVEL_RIVER_MASK, SCREEN_WIDTH);

	Plane plane;

   	plane.sur = surfaces[SUR_PLANE];
	plane.x = (SCREEN_WIDTH/2) - (plane.sur->w/2);
	plane.level_y = level.sur->h - plane.sur->h;

	plane.true_y = SCREEN_HEIGHT - plane.sur->h;
	plane.speed = PLANE_START_SPEED;

	plane.boxes.tab = NULL;

	Rect rect;

	rect.w = 6;
	rect.h = 10;
	rect.x = plane.x + 21;
	rect.y = plane.level_y - plane.sur->w;
	vect_add(rect, &plane.boxes);

	rect.w = 44;
	rect.h = 7;
	rect.x = plane.x + 2;
	rect.y = plane.level_y - plane.sur->w - 6;
	vect_add(rect, &plane.boxes);

	rect.w = 6;
	rect.h = 18;
	rect.x = plane.x + 21;
	rect.y = plane.level_y - plane.sur->w - 17;
	vect_add(rect, &plane.boxes);

	SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	while (quit == 0) {
		//Start teh frame timer
		start_ticks = SDL_GetTicks();

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {

			handle_plane_input(&plane, &level, &e);

			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = 1;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				}
			}
		}

		move_plane(&plane, &level);
		set_camera(&camera, &level, &plane);

		if (land_collision(&plane, &level)) {
			SDL_Delay( 4000 );
			break;
		}

		apply_surface(0, 0, level.sur, screen, &camera);

		show_plane(&plane, screen);

		SDL_UpdateWindowSurface(window);

		int ticks = SDL_GetTicks() - start_ticks;
		if (ticks < 1000/FRAMES_PER_SECOND) {
			SDL_Delay((1000/FRAMES_PER_SECOND) - ticks);
		}
	}

	free_media_all(surfaces, levels);

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
