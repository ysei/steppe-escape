#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>
#include <SDL/SDL_ttf.h>

#include "utils.h"
#include "level.h"

int
part_of_rect(char ch, char *xpm[], size_t i, size_t j) {

	if (i == 0 || (i > 0 && xpm[i-1][j] != ch)) {
		if (j == strlen(xpm[i])-1 || xpm[i][j+1] != ch) {
			return 1;
		}
	}

	if (j > 0 && xpm[i][j-1] == ch) {
		return 2;
	} else {
		if (i > 0 && xpm[i-1][j] == ch) {
			return 2;
		}
		return 0;
	}
}

void
load_level(Level *level, char *xpm[], Rect_Vect *overlords, RGB_Color color, size_t screen_width) {
	size_t width, height, colors, chars_on_px;
	sscanf(xpm[0], "%zu %zu %zu %zu", &width, &height, &colors, &chars_on_px);
	if (width != screen_width) {
		error("load_level: map width(%d) should be the same as screen_width(%d)\n", width, screen_width);
	} else if (chars_on_px != 1) {
		error("load_level: only one char on pixel allowed in map format\n");
	}
	char land_ch = '\0';
	char overlord_ch = '\0';
	char movelord_ch = '\0';
	for (int i = 1; i <= colors; i++) {
		//I assume here that colors are in format: #XXXXXX
		char ch, code[8];
		sscanf(xpm[i], "%1c %*c %s", &ch, code);
		if (strcmp(code, "#000000") == 0) {
			land_ch = ch;
		} else if (strcmp(code, "#FF0000") == 0){
			overlord_ch = ch;
		} else if (strcmp(code, "#FFFF00") == 0){
			movelord_ch = ch;
		}
	}
	if (land_ch == '\0') {
		error("load_level: cannot find LEVEL_RIEVR_MASK in mapfile(%d, %d, %d)", color.r, color.g, color.b);
	}

	level->boxes.tab = NULL;

	overlords->tab = NULL;
	overlords->size = 0;

	size_t lv_start_line = colors + 1;
	int movelord_start = -1;
	for (size_t i = lv_start_line; i < height + lv_start_line; i++) {

		size_t w = 0, start_x = 0;
		for (size_t j = 0; j < strlen(xpm[i]); j++) {
			Rect rect;
			if (xpm[i][j] == land_ch) {
				w++;
			} else {
				if (w > 0) {
					rect.w = w;
					rect.h = 1;
					rect.x = start_x;
					rect.y = i - lv_start_line;
					vect_add(rect, &level->boxes);
					w = 0;
					start_x = j;
				} 
				start_x++;
				if (xpm[i][j] == overlord_ch && !part_of_rect(overlord_ch, xpm, i, j)) {
					Rect overlord;
					overlord.x = j + 5;
					overlord.y = i - lv_start_line + 5;
					overlord.w = 38;
					overlord.h = 38;
					overlord.from = overlord.to = 0;
					vect_add(overlord, overlords);
					printf("overlord: %zu, %zu\n", overlord.x, overlord.y);
				} else if (xpm[i][j] == movelord_ch) {
					int part = part_of_rect(movelord_ch, xpm, i, j);

					if (part == 0) {
						movelord_start = j;
					//end of the first line of rentangle
					} else if (part == 1) {
						Rect overlord;
						overlord.x = movelord_start + 5;
						overlord.y = i - lv_start_line + 5;
						overlord.w = 38;
						overlord.h = 38;

						overlord.from = overlord.x;
						overlord.to = j - overlord.w;
						vect_add(overlord, overlords);
						printf("movelord: x: %zu, y: %zu start: %d, end: %d\n", overlord.x, overlord.y, overlord.from, overlord.to);
					}
				}
			}
		}
		if (w != 0) {
			Rect rect;
			rect.w = w;
			rect.h = 1;
			rect.x = start_x;
			rect.y = i - lv_start_line;
			vect_add(rect, &level->boxes);
		}
	}
}

Speeches *
load_speech(char *speech, long *line, Speeches *last) {
	Speeches *next;

	if (last != NULL) {
		strcpy(speech, last->text);
		*line = last->line;

		next = last->next;
		free(last);
		return next;

	} else {
		*line = -1;
		return NULL;
	}
}

//return new_nlast;
Speeches *
add_speech(long line, char *speech, Speeches *last) {
	Speeches *new;
	new = emalloc(sizeof(Speeches));
	new->text = speech;
	new->line = line;
	new->next = NULL;
	if (last != NULL) {
		last->next = new;
	} 

	return new;
}
