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
change_plane_boxes(Plane *plane) {

	vect_clean(&plane->boxes);

	Rect rect;

	rect.w = 6;
	rect.h = 10;
	rect.x = plane->x + 21;
	rect.y = plane->level_y;
	vect_add(rect, &plane->boxes);

	rect.w = 44;
	rect.h = 7;
	rect.x = plane->x + 2;
	rect.y = plane->level_y + 6;
	vect_add(rect, &plane->boxes);

	rect.w = 6;
	rect.h = 18;
	rect.x = plane->x + 21;
	rect.y = plane->level_y + 17;
	vect_add(rect, &plane->boxes);

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

	change_plane_boxes(plane);
}

int
collision_detect(Rect_Vect *a, Rect_Vect *b) {
	size_t left_a, left_b;
	size_t right_a, right_b;
	size_t top_a, top_b;
	size_t bottom_a, bottom_b;

	vect_print(b);

	for (size_t i = 0; i < a->size; i++) {
		left_a = a->tab[i].x;
		right_a = a->tab[i].x + a->tab[i].w;
		top_a = a->tab[i].y;
		bottom_a = a->tab[i].y + a->tab[i].h;
		for (size_t j = 0; j < b->size; j++) {
			left_b = b->tab[j].x;
			right_b = b->tab[j].x + b->tab[j].w;
			top_b = b->tab[j].y;
			bottom_b = b->tab[j].y + b->tab[j].h;

			/*if (!(bottom_a <= top_b || top_a >= bottom_b)) {
			}*/

			//if ((bottom_a <= top_b && top_a <= bottom_b) && (left_b <= right_a && right_b >= left_a)) {
			if ( ! ( ( bottom_a <= top_b ) || ( top_a >= bottom_b ) || ( right_a <= left_b ) || ( left_a >= right_b ) ) ) {
				printf("%zu. %zu %zu %zu %zu\n",i, left_a, right_a, top_a, bottom_a);
				printf("%zu %zu %zu %zu\n\n", left_b, right_b, top_b, bottom_b);
				return 1;
			}
		}
	}
	return 0;
}

int
land_collision(Plane *plane, Level *level, size_t *last_pos) {
	//check only positions that matters
	Rect_Vect strip;
	strip.tab = NULL;
	strip.size = 0;
	strip.max  = 0;

	for (size_t pos = *last_pos; pos >= 0 && level->boxes.tab[pos].y >= plane->boxes.tab[0].y; pos--) {
		vect_add(level->boxes.tab[pos], &strip);
	}

	return collision_detect(&plane->boxes, &strip);
}

void
wait_for_space() {
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

	//waiting to start the game
	wait_for_space();

	int quit = 0;

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
	change_plane_boxes(&plane);


	SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	SDL_Event e;
	size_t last_land_pos = level.boxes.size - 1;
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

		apply_surface(0, 0, level.sur, screen, &camera);

		show_plane(&plane, screen);

		SDL_UpdateWindowSurface(window);

		if (land_collision(&plane, &level, &last_land_pos)) {
			wait_for_space();
			break;
		}


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
