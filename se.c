#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include "utils.h"
#include "config.h"
#include "level.h"
#include "se.h"

#include "levels/l1.xpm"
#include "levels/l2.xpm"

SDL_Surface *
load_media(const char *src, SDL_Surface *screen) {
	int detect;
	SDL_Surface *img, *opt_img;

	detect = ext(src);
	img = IMG_Load(src);
	if (img == NULL) {
		error("Unable to load image %s! SDL Error: %s\n", src, SDL_GetError());
	}

	if (detect == EXT_BMP) {
		opt_img = SDL_ConvertSurface(img, screen->format, 0);
		if (opt_img == NULL) {
			error("Unable to optimize image %s! SDL Error: %s\n", src, SDL_GetError());
		}
		//Get rid of unoptimalized surface
		SDL_FreeSurface(img);

		Uint32 color_key = SDL_MapRGB(opt_img->format, 0, 0xFF, 0xFF);
		SDL_SetColorKey(opt_img, SDL_TRUE, color_key);

		return opt_img;
	} else if (detect == EXT_PNG) {
		return img;
	}
	return NULL;
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
set_camera(SDL_Rect *camera, Level *level, Plane *plane, int *road_in_lv) {
	*road_in_lv += plane->speed;

	camera->y = level->sur->h - SCREEN_HEIGHT - *road_in_lv;

	if (camera->y < 0) {
		camera->y = 0;
	}
}

void
show_plane(Plane *plane, SDL_Surface *screen) {
	apply_surface(plane->x, plane->true_y, plane->sur, screen, NULL);
}

//Returns actual plane surfce
int 
handle_plane_input(Plane *plane, Level *level, SDL_Event *e) {
	if (e->type == SDL_KEYDOWN) {
		switch(e->key.keysym.sym) {
			case SDLK_LEFT:
				plane->x -= PLANE_X_SPEED;
				return SUR_PLANE_L;
			case SDLK_RIGHT:
				plane->x += PLANE_X_SPEED;
				return SUR_PLANE_R;

			case SDLK_UP:
				plane->speed += PLANE_SPEED_MULTIPLAYER;
				if (plane->speed > PLANE_SPEED_MAX) {
					plane->speed = PLANE_SPEED_MAX;
				}
				return SUR_PLANE;
			case SDLK_DOWN:
				plane->speed -= PLANE_SPEED_MULTIPLAYER;
				if (plane->speed < PLANE_SPEED_MIN) {
					plane->speed = PLANE_SPEED_MIN;
				}
				return SUR_PLANE;
		}
	} 
	return SUR_PLANE;
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

	plane->level_y -= plane->speed;
	if (plane->level_y < plane->true_y) {
		plane->true_y = plane->level_y;
	}

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

	for (size_t pos = *last_pos; pos >= 0 && pos < level->boxes.size && level->boxes.tab[pos].y >= plane->boxes.tab[0].y; pos--) {
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
					case SDLK_RETURN:
						quit = 1;
					break;
				}
			}
		}
	}
}

void
redraw_menu(SDL_Surface *screen, TTF_Font *fonts[], SDL_Surface *surfaces[], SDL_Rect *screen_rect, char *texts[], size_t texts_len, size_t active_text) {
	SDL_Color text_color = {0xFF, 0xFF, 0xFF};
	int menu_top_margin = 220;
	int between_margin  = 30; 
	int menu_left_margin = 150;

	SDL_BlitScaled(surfaces[SUR_START], NULL, screen, screen_rect);

	for (size_t i = 0; i < texts_len; i++) {
		SDL_Surface *text_sur;
		if (i == active_text) {
			TTF_SetFontOutline(fonts[FONT_MENU], 1);
		} else {
			TTF_SetFontOutline(fonts[FONT_MENU], 0);
		}
		text_sur = TTF_RenderUTF8_Blended(fonts[FONT_MENU], texts[i], text_color);
		apply_surface(menu_left_margin, menu_top_margin, text_sur, screen, NULL);
		menu_top_margin += text_sur->h + between_margin;
		SDL_FreeSurface(text_sur);
	}

}

void
init_anim(Animation *anim) {
	anim->dir = 1;

	anim->frame = rand() % 4;

	printf("start_frame: %d\n", anim->frame);

	anim->fpa = 15;
	anim->fpa_count = 0;
	anim->clips_r.tab = NULL;
	for (int i = 0; i < 4; i++) {
		Rect rect;
		rect.x = 0 + 48*i;
		rect.y = 0;
		rect.w = 48;
		rect.h = 48;
		vect_add(rect, &anim->clips_r);
	}
}

void
show_anim(int x, int y, SDL_Surface *source, SDL_Surface *dest, Animation *anim) {
	if (anim->frame >= 4)
		anim->frame = 0;
	Rect clip = anim->clips_r.tab[anim->frame];

	SDL_Rect sdl_rect;
	sdl_rect.x = clip.x;
	sdl_rect.y = clip.y;
	sdl_rect.w = clip.w;
	sdl_rect.h = clip.h;

	apply_surface(x, y, source, dest, &sdl_rect);
	if (anim->fpa_count <= 0) {
		anim->frame++;
		anim->fpa_count = anim->fpa;
	} else {
		anim->fpa_count--;
	}

}


void
init_speeches(Speeches *speeches[]) {
	Speeches *sp = NULL;

	sp = add_speech(0, "No to do boju! Początek powinien być łatwy. Wystarczy, że nie będę zwracał na siebie uwagi i jescze dzisiaj spokojnie wylonduję w Karagandzie.", sp);
	speeches[1] = sp;
	sp = add_speech(0, "Martwy mnie tylko niski stan paliwa. Muszę coś z tym zrobić.", sp);
	sp = add_speech(320, "Och, było blisko!", sp);

	sp = NULL;
	sp = add_speech(0, "Jakoś nie poznaje tego miejsca.", sp);
	speeches[2] = sp;
	sp = add_speech(0, "Ciekawe gdzie jestem?", sp);
	sp = add_speech(320, "To Overlordy mogą latać!?", sp);
	sp = add_speech(640, "Benzyna. Ciekawe co tu robi?!", sp);
	sp = add_speech(640, "Być może to jakiś przechwycony kazachski okręt! Muszę to sprawdzić.", sp);

}

//should return SDL_Surface in order to proper cleanning
void 
pilot_say(char *text, TTF_Font *fonts[], SDL_Surface *surfaces[], SDL_Surface *screen) {
		SDL_Surface *pilot = surfaces[SUR_PILOT];
		SDL_Surface *rect_sur;

		int rect_height = 130;
		int margin_left = 20;
		int margin_top = 10;
		int text_max_width = SCREEN_WIDTH - 2*margin_left - pilot->w;

		/*SDL_Rect sdl_rect;
		sdl_rect.x = 0;
		sdl_rect.y = SCREEN_HEIGHT - rect_height;
		sdl_rect.w = SCREEN_WIDTH;
		sdl_rect.h = rect_height;*/

		
		rect_sur = SDL_CreateRGBSurface(0, SCREEN_WIDTH, rect_height, 32, 0, 0, 0, 0);
		apply_surface(0, SCREEN_HEIGHT - rect_height, rect_sur, screen, NULL);

		/*SDL_SetSurfaceAlphaMod(screen, 255);
		SDL_SetSurfaceBlendMode(screen, SDL_BLENDMODE_NONE);
		SDL_FillRect(screen, &sdl_rect, SDL_MapRGBA(screen->format, 0, 0, 0, 100));*/

		SDL_Color text_color = {0xFF, 0xFF, 0xFF};

		char line[200];
		char new_line[200];
		char word[100];

		strcpy(line, text);

		int line_nr = 0;
		new_line[0] = '\0';
		while (line[0] != '\0') {
			SDL_Surface *text_sur = TTF_RenderUTF8_Blended(fonts[FONT_PILOT_SAY], line, text_color);
			if (text_sur->w > text_max_width) {
				get_last_word(line, word);
				printf("strcat(new_line, word):%s, %s\n", new_line, word);
				strbefore(word, new_line);
			} else {
				printf("Pilot say:%s\n", line);
				apply_surface(pilot->w + margin_left, \
						SCREEN_HEIGHT - rect_height + margin_top + line_nr * text_sur->h, \
					   	text_sur, screen, NULL);
				//remove first space from new_line
				memmove(new_line, new_line + 1, strlen(new_line)*sizeof(char));
				strcpy(line, new_line);
				new_line[0] = '\0';
				line_nr++;
			}
		}

		//apply_surface(pilot->w + text_margin, SCREEN_H - 600, text_sur, screen, NULL);

		apply_surface(10, SCREEN_HEIGHT - pilot->h, pilot, screen, NULL);
}

void
game_over(long points) {
	Mix_Music *music;
	if ((music = Mix_LoadMUS("sounds/game_over.ogg")) == NULL) {
	     error("Mix_LoadMUS: %s\n", Mix_GetError());
	}
	Mix_PlayMusic(music, -1);
	wait_for_space();
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
	//Initiate SDL_Image
	//
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        error("Could not init SDL_Image: %s", IMG_GetError());
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

menu:
	if ((music = Mix_LoadMUS("sounds/Betty_Roche-Trouble_Trouble.ogg")) == NULL) {
	     error("Mix_LoadMUS: %s\n", Mix_GetError());
	}

	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = SCREEN_WIDTH;
	screen_rect.h = SCREEN_HEIGHT;



	char *texts[] = {"Nowa gra", "Surwiwal", "Koniec"};
	int active_text = 0;
	redraw_menu(screen, fonts, surfaces, &screen_rect, texts, sizeof(texts)/sizeof(char *), active_text);
	SDL_UpdateWindowSurface(window);

	Mix_PlayMusic(music, -1);

	//waiting to start the game
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
					case SDLK_ESCAPE:
						goto exit;
					break;
					case SDLK_RETURN:
						quit = 1;
					break;
					case SDLK_UP:
						active_text--;
						if (active_text < 0)
							active_text = sizeof(texts)/sizeof(char *) - 1;

						redraw_menu(screen, fonts, surfaces, &screen_rect, texts, sizeof(texts)/sizeof(char *), active_text);
						SDL_UpdateWindowSurface(window);
					break;
					case SDLK_DOWN:
						active_text++;
						if (active_text > sizeof(texts)/sizeof(char *) - 1)
							active_text = 0;
						redraw_menu(screen, fonts, surfaces, &screen_rect, texts, sizeof(texts)/sizeof(char *), active_text);
						SDL_UpdateWindowSurface(window);
					break;
				}
			}
		}
	}
	switch (active_text) {
		case 2:
			goto exit;
			break;
	}

	quit = 0;


	int level_nr = 1;

	//+1 becouse we don't have lv 0;
	Speeches *speeches[LEVEL_TOTAL + 1];
	init_speeches(speeches);

	long points = 0;
	long ammo = 10;
	long distance = 0;
	//how many pixel 
	long fuel = 8000;
	long chanses = 3;

	Mix_PlayChannel(-1, sounds[SOUND_ENGINE], -1);
	//levels begin from 1
	//level loop
	while (level_nr < LEVEL_TOTAL+1) {
		long start_ammo = ammo;
		long start_distance = distance;
		long start_fuel = fuel;

		//loading level
		Level level;
		Rect_Vect overlords;
		Rect_Vect fuels;

		switch (level_nr) {
			case 1:
				level.sur = levels[LEVEL_1];
				load_level(&level, l1_xpm, &overlords, &fuels, LEVEL_RIVER_MASK, SCREEN_WIDTH);
				//end loading level
				break;
			case 2:
				level.sur = levels[LEVEL_2];
				load_level(&level, l2_xpm, &overlords, &fuels, LEVEL_RIVER_MASK, SCREEN_WIDTH);
				//end loading level
				break;
		}

		Plane plane;

		plane.sur = surfaces[SUR_PLANE];
		plane.x = (SCREEN_WIDTH/2) - (plane.sur->w/2);
		plane.level_y = level.sur->h - plane.sur->h;

		plane.true_y = SCREEN_HEIGHT - plane.sur->h;
		plane.speed = PLANE_START_SPEED;

		plane.boxes.tab = NULL;
		change_plane_boxes(&plane);


		SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

		size_t last_land_pos = level.boxes.size - 1;

		Rect_Vect bullets;
		bullets.tab = NULL;
		bullets.size = 0;

		Animation *ov_anims;
		ov_anims = emalloc(overlords.size * sizeof(Animation));

		for (int i = 0; i < overlords.size; i++) {
			init_anim(&ov_anims[i]);
		}

		//reloading farmes
		int reload = (1000/(THEORETICAL_RATE/60)) / FRAMES_PER_SECOND;
		int frames_to_shoot = 0;


		int act_sur;

		long next_speech;
		char speech[300];
		speeches[level_nr] = load_speech(speech, &next_speech, speeches[level_nr]);

		int start_ticks;
		int road_in_lv = 0;
		int collision = 0;
		//MAIN LOOP
		while (quit == 0) {
			//Start teh frame timer
			start_ticks = SDL_GetTicks();

			//Handle events on queue
			while (SDL_PollEvent(&e) != 0) {

				act_sur = handle_plane_input(&plane, &level, &e);
				plane.sur = surfaces[act_sur];

				//User requests quit
				if (e.type == SDL_QUIT) {
					quit = 1;
				} else if (e.type == SDL_KEYDOWN) {
					Rect bullet;
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							goto exit;
						break;
						case SDLK_SPACE:
							if (frames_to_shoot <= 0) {
								if (ammo > 0) {
									Mix_PlayChannel(-1, sounds[SOUND_GUN], 0);
									bullet.w = 1;
									bullet.h = 10;
									bullet.x = plane.x + plane.sur->w/2 - 15;
									bullet.y = plane.level_y;
									vect_add(bullet, &bullets);

									bullet.w = 1;
									bullet.h = 10;
									bullet.x = plane.x + plane.sur->w/2 + 15;
									bullet.y = plane.level_y;
									vect_add(bullet, &bullets);
									frames_to_shoot = reload;
									ammo--;
								} else {
									Mix_PlayChannel(-1, sounds[SOUND_EMPTY_SHOOT], 0);
								}
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

			distance += plane.speed;
			fuel -= plane.speed;

			set_camera(&camera, &level, &plane, &road_in_lv);

			apply_surface(0, 0, level.sur, screen, &camera);

			//show fuel
			for (size_t i = 0; i < fuels.size; i++) {
				size_t true_y;
				true_y = fuels.tab[i].y - (plane.level_y + plane.sur->h) + SCREEN_HEIGHT; 

				apply_surface(fuels.tab[i].x, true_y, surfaces[SUR_FUEL], screen, NULL);
			}
			if (collision_detect(&plane.boxes, &fuels, NULL, NULL)) {
				fuel += FUEL_LOADING_SPEED;
				Mix_PlayChannel(-1, sounds[SOUND_BLUP], 0);
				if (fuel >= MAX_FUEL) {
					fuel = MAX_FUEL;
				}
			}

			size_t fuel_box, bul_ful_box;
			if (collision_detect(&fuels, &bullets, &fuel_box, &bul_ful_box)) {
				Mix_PlayChannel(-1, sounds[SOUND_FUEL_EXPLOSION], 0);
				vect_del(fuel_box, &fuels);
				vect_del(bul_ful_box, &bullets);
				points += 10;
				printf("%zu fuel shooted\n", fuel_box);
			}

			show_plane(&plane, screen);

			//show bullets
			for (size_t i = 0; i < bullets.size; i++) {
				int true_y;
				if (plane.level_y != plane.true_y) {
					true_y = bullets.tab[i].y - (plane.level_y + plane.sur->h) + SCREEN_HEIGHT; 
				} else {
					true_y = bullets.tab[i].y;
				}

				if (true_y < 0) {
					vect_del(i, &bullets);
				} else {
					SDL_Rect bul_rect = {bullets.tab[i].x, true_y, bullets.tab[i].w ,bullets.tab[i].h};
					SDL_FillRect(screen, &bul_rect, SDL_MapRGB(screen->format, 0, 0, 0));
				}
			}

			for (size_t i = 0; i < overlords.size; i++) {
				size_t true_y;
				true_y = overlords.tab[i].y - (plane.level_y + plane.sur->h) + SCREEN_HEIGHT; 
				//if movelord
				if  (overlords.tab[i].to != 0 ) {
					overlords.tab[i].x += ov_anims[i].dir * OVERLORD_SPEED;
					if (overlords.tab[i].x > overlords.tab[i].to) {
						overlords.tab[i].x = overlords.tab[i].to;
						ov_anims[i].dir = -1;
					} else if (overlords.tab[i].x < overlords.tab[i].from) {
						overlords.tab[i].x = overlords.tab[i].from;
						ov_anims[i].dir = 1;
					}
				}
				SDL_Surface *dir_sur;
				if (ov_anims[i].dir == 1) {
					dir_sur = surfaces[SUR_OVERLORD];
				} else {
					dir_sur = surfaces[SUR_OVERLORD_R];
				}
				show_anim(overlords.tab[i].x, true_y, dir_sur, screen, &ov_anims[i]);

			}


			//update com
			SDL_Color text_color = {0, 0, 0};
			SDL_Color text_high = {0, 0xFF, 0};
			SDL_Color text_medium = {0, 0xFF, 0xFF};
			SDL_Color text_low = {0xFF, 0, 0};

			int margin_bottom = 4;
			SDL_Surface *text_sur;
			char text[100];
			int r_sur_w = 0;

			sprintf(text, "Punkty: %ld", points);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(0, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			r_sur_w += text_sur->w + 25;
			SDL_FreeSurface(text_sur);

			sprintf(text, "Amunicja: %ld", ammo);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(r_sur_w, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			r_sur_w += text_sur->w + 25;
			SDL_FreeSurface(text_sur);


			sprintf(text, "Paliwo: ");
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(r_sur_w, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			r_sur_w += text_sur->w;
			SDL_FreeSurface(text_sur);

			SDL_Color color;
			int fuel_percent = (fuel*100)/MAX_FUEL;
			if (fuel_percent > 60)  {
				color = text_high;
			} else if (fuel_percent > 30) {
				color = text_medium;
			} else {
				color = text_low;
			}
			sprintf(text, "%d%%", fuel_percent);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, color);
			apply_surface(r_sur_w, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			SDL_FreeSurface(text_sur);


			int speed_sur_w = 0;
			//should be: double km_h_speed = ((plane.speed * PIXEL_SCALE)/1000) / ((1000/FRAMES_PER_SECOND)/(1000*3600));
			double km_h_speed = ((plane.speed * PIXEL_SCALE)/1000)*(1000*3600) / (1000/FRAMES_PER_SECOND);

			sprintf(text, "Prędkość: %.f km/h", km_h_speed);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(SCREEN_WIDTH - text_sur->w - 6, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			speed_sur_w += text_sur->w + 25;


			SDL_FreeSurface(text_sur);


			double distance_km = (distance * PIXEL_SCALE)/1000;
			sprintf(text, "Pokonana odległość: %.2f km", distance_km);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(SCREEN_WIDTH - speed_sur_w - text_sur->w, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			speed_sur_w += text_sur->w + 25;
			SDL_FreeSurface(text_sur);

			sprintf(text, "Pozostałe szanse: %ld", chanses);
			text_sur = TTF_RenderUTF8_Solid(fonts[FONT_INFO], text, text_color);
			apply_surface(SCREEN_WIDTH - speed_sur_w - text_sur->w, SCREEN_HEIGHT - Fonts_Sizes[FONT_INFO] - margin_bottom, text_sur, screen, NULL);
			speed_sur_w += text_sur->w;
			SDL_FreeSurface(text_sur);



			//printf("%d == %ld\n", plane.level_y + plane.sur->h, (level.sur->h - next_speech));
			while (plane.level_y <= (level.sur->h - next_speech) && next_speech != -1) {
				pilot_say(speech, fonts, surfaces, screen);
				speeches[level_nr] = load_speech(speech, &next_speech, speeches[level_nr]);
				SDL_UpdateWindowSurface(window);
				wait_for_space();
			}
			SDL_UpdateWindowSurface(window);

			if (fuel <= 0) {
				collision = 1;
			}

			if (land_collision(&plane, &level, &last_land_pos)) {
				collision = 1;
			}

			if (collision_detect(&plane.boxes, &overlords, NULL, NULL)) {
				collision = 1;
			}

			if (collision == 1) {
				chanses -= 1;
				Mix_PlayChannel(-1, sounds[SOUND_BIG_EXPLOSION], 0);

				if (chanses < 0) {
					Mix_HaltChannel(-1);
					game_over(points);
					goto menu;
				} else {
					ammo = start_ammo;
					distance = start_distance;
					fuel = start_fuel;
					wait_for_space();
					break;
				}
			}

			size_t ov_box, bul_box;
			if (collision_detect(&overlords, &bullets, &ov_box, &bul_box)) {
				Mix_PlayChannel(-1, sounds[SOUND_OV_DEATH], 0);
				vect_del(ov_box, &overlords);
				vect_del(bul_box, &bullets);
				points += 10;
				printf("%zu overlord shooted\n", ov_box);
			}
			


			int ticks = SDL_GetTicks() - start_ticks;
			if (ticks < 1000/FRAMES_PER_SECOND) {
				SDL_Delay((1000/FRAMES_PER_SECOND) - ticks);
			}
			if (frames_to_shoot > 0)
				frames_to_shoot--;

			//check if level over
			if (plane.level_y + plane.sur->h <= 0) {
				printf("!! LEVEL OVER !!");
				level_nr++;
				//cleaning
				SDL_FreeSurface(level.sur);
				break;
			}
		}//end game loop
	}//end level loop

exit:
	free_media_all(surfaces, levels, fonts, sounds);

	SDL_DestroyWindow(window);
	window = NULL;

	Mix_FreeMusic(music);
	Mix_CloseAudio();

	TTF_Quit();

	SDL_Quit();


	exit(EXIT_SUCCESS);
}
