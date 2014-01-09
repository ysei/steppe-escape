#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

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
load_media_all(SDL_Surface *surfaces[], SDL_Surface *levels[], TTF_Font *fonts[], Mix_Chunk *sounds[], SDL_Surface *screen) {
	char path[MAX_PATH_LEN]; 

	for (int i = 0; i < SUR_TOTAL; i++) {

		add_slash_to_path(path, IMAGE_PATH);
		strcat(path, Images[i]);

		if ((surfaces[i] = load_media(path, screen)) == NULL) {
			error("load_media_all: Cannot open surface: %s\n", path);
		}
	}

	for (int i = 0; i < LEVEL_TOTAL; i++) {

		add_slash_to_path(path, LEVEL_PATH);

		char l_name[MAX_PATH_LEN];
		sprintf(l_name, "%s%d.bmp", LEVEL_PREFIX, i+1);

		strcat(path, l_name);

		if ((levels[i] = load_media(path, screen)) == NULL) {
			error("load_media_all: Cannot open level: %s\n", path);
		}
	}
	
	//fonts
	for (int i = 0; i < FONT_TOTAL; i++) {

		add_slash_to_path(path, FONT_PATH);
		strcat(path, Fonts[i]);

		if ((fonts[i] = TTF_OpenFont(path,  Fonts_Sizes[i])) == NULL) {
			error("load_media_all: Cannot open font: %s: %s\n", path, TTF_GetError());
		}
	}

	for (int i = 0; i < SOUND_TOTAL; i++) {

		add_slash_to_path(path, SOUND_PATH);
		strcat(path, Sounds[i]);

		if ((sounds[i] = Mix_LoadWAV(path)) == NULL) {
			error("load_media_all: Cannot load sound: %s: %s\n", path, Mix_GetError());
		}
	}
}

void
free_media_all(SDL_Surface *surfaces[], SDL_Surface *levels[], TTF_Font *fonts[], Mix_Chunk *sounds[]) {
	for (int i = 0; i < SUR_TOTAL; i++) {
		free_media(surfaces[i]);
	}
	for (int i = 0; i < LEVEL_TOTAL; i++) {
		free_media(levels[i]);
	}
	//fonts
	for (int i = 0; i < FONT_TOTAL; i++) {
		TTF_CloseFont(fonts[i]);
	}

	for (int i = 0; i < SOUND_TOTAL; i++) {
		Mix_FreeChunk(sounds[i]);
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

size_t
collision_detect(Rect_Vect *a, Rect_Vect *b, size_t *ind_a, size_t *ind_b) {
	size_t left_a, left_b;
	size_t right_a, right_b;
	size_t top_a, top_b;
	size_t bottom_a, bottom_b;

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

			if ( ! ( ( bottom_a <= top_b ) || ( top_a >= bottom_b ) || ( right_a <= left_b ) || ( left_a >= right_b ) ) ) {
				printf("%zu. %zu %zu %zu %zu\n", i, left_a, right_a, top_a, bottom_a);
				printf("%zu. %zu %zu %zu %zu\n\n", j, left_b, right_b, top_b, bottom_b);
				if (ind_a != NULL) 
					*ind_a = i;
				if (ind_b != NULL) 
					*ind_b = j;
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

	return collision_detect(&plane->boxes, &strip, NULL, NULL);
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

	//Initialize SDL_ttf
	if (TTF_Init() == -1) {
		error("Could not initialise sdl_ttf.");
	}
	//Initialize SDL_mixer
	if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		error("Could not initialise sdl_mixer: %s", Mix_GetError());
	}

	// load support for the OGG  sample/music formats
	int mix_flags = MIX_INIT_OGG;
	int mix_initted = Mix_Init(mix_flags);
	if ((mix_initted & mix_flags) != mix_flags) {
	     error("Mix_Init: Failed to init required ogg support: %s\n", Mix_GetError());
	}

	screen = SDL_GetWindowSurface(window);

	TTF_Font *fonts[FONT_TOTAL];
	Mix_Chunk *sounds[SOUND_TOTAL];
	Mix_Music *music;

	load_media_all(surfaces, levels, fonts, sounds, screen);

	if ((music = Mix_LoadMUS("sounds/Betty_Roche-Trouble_Trouble.ogg")) == NULL) {
	     error("Mix_LoadMUS: %s\n", Mix_GetError());
	}

	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = SCREEN_WIDTH;
	screen_rect.h = SCREEN_HEIGHT;

	SDL_BlitScaled(surfaces[SUR_START], NULL, screen, &screen_rect);

	SDL_UpdateWindowSurface(window);

	Mix_PlayMusic(music, -1);
	//waiting to start the game
	wait_for_space();

	int quit = 0;

	int start_ticks;

	Level level;
	level.sur = levels[LEVEL_1];

	Rect_Vect overlords;
	load_level(&level, l1_xpm, &overlords, LEVEL_RIVER_MASK, SCREEN_WIDTH);

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

	Rect_Vect bullets;
	bullets.tab = NULL;
	bullets.size = 0;

	//reloading farmes
	int reload = (1000/(THEORETICAL_RATE/60)) / FRAMES_PER_SECOND;
	int frames_to_shoot = 0;

	Mix_PlayChannel(-1, sounds[SOUND_ENGINE], -1);
	while (quit == 0) {
		//Start teh frame timer
		start_ticks = SDL_GetTicks();

		printf("%d\n", frames_to_shoot);
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {

			handle_plane_input(&plane, &level, &e);

			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = 1;
			} else if (e.type == SDL_KEYDOWN) {
				Rect bullet;
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						if (frames_to_shoot <= 0) {
							Mix_PlayChannel(-1, sounds[SOUND_GUN], 0);
							bullet.w = 1;
							bullet.h = 10;
							bullet.x = plane.x + plane.sur->w/2;
							bullet.y = plane.level_y;
							vect_add(bullet, &bullets);
							frames_to_shoot = reload;
						}
						break;
				}
			}
		}
		//move bullets
		for (size_t i = 0; i < bullets.size; i++) {
			bullets.tab[i].y -= BULLET_SPEED;
		}

		move_plane(&plane, &level);
		set_camera(&camera, &level, &plane);

		apply_surface(0, 0, level.sur, screen, &camera);

		show_plane(&plane, screen);

		for (size_t i = 0; i < bullets.size; i++) {
			size_t true_y;
			true_y = bullets.tab[i].y - (plane.level_y + plane.sur->h) + SCREEN_HEIGHT; 

			SDL_Rect bul_rect = {bullets.tab[i].x, true_y, bullets.tab[i].w ,bullets.tab[i].h};
			SDL_FillRect(screen, &bul_rect, SDL_MapRGB(screen->format, 0, 0, 0));
		}

		for (size_t i = 0; i < overlords.size; i++) {
			size_t true_y;
			true_y = overlords.tab[i].y - (plane.level_y + plane.sur->h) + SCREEN_HEIGHT; 
			apply_surface(overlords.tab[i].x, true_y, surfaces[SUR_OVERLORD], screen, NULL);
		}

		SDL_UpdateWindowSurface(window);

		if (land_collision(&plane, &level, &last_land_pos)) {
			goto gameover;
		}

		if (collision_detect(&plane.boxes, &overlords, NULL, NULL)) {
			goto gameover;
		}

		size_t ov_box, bul_box;
		if (collision_detect(&overlords, &bullets, &ov_box, &bul_box)) {
			Mix_PlayChannel(-1, sounds[SOUND_OV_DEATH], 0);
			vect_del(ov_box, &overlords);
			vect_del(bul_box, &bullets);
			printf("%zu overlord shooted\n", ov_box);
		}
		


		int ticks = SDL_GetTicks() - start_ticks;
		if (ticks < 1000/FRAMES_PER_SECOND) {
			SDL_Delay((1000/FRAMES_PER_SECOND) - ticks);
		}
		if (frames_to_shoot > 0)
			frames_to_shoot--;
	}

gameover:

	Mix_PlayChannel(-1, sounds[SOUND_BIG_EXPLOSION], 0);

	wait_for_space();

	free_media_all(surfaces, levels, fonts, sounds);

	SDL_DestroyWindow(window);
	window = NULL;

	Mix_FreeMusic(music);
	Mix_CloseAudio();

	TTF_Quit();

	SDL_Quit();


	exit(EXIT_SUCCESS);
}
