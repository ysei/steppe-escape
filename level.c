#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>

#include "utils.h"
#include "level.h"

int
part_of_rect(char ch, char *xpm[], size_t i, size_t j) {
	if (j > 0 && xpm[i][j-1] == ch) {
		return 1;
	} else {
		if (i > 0 && xpm[i-1][j] == ch) {
			return 1;
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
	for (int i = 1; i <= colors; i++) {
		//I assume here that colors are in format: #XXXXXX
		char ch, code[8];
		sscanf(xpm[i], "%1c %*c %s", &ch, code);
		if (strcmp(code, "#000000") == 0) {
			land_ch = ch;
		} else if (strcmp(code, "#FF0000") == 0){
			overlord_ch = ch;
		}
	}
	if (land_ch == '\0') {
		error("load_level: cannot find LEVEL_RIEVR_MASK in mapfile(%d, %d, %d)", color.r, color.g, color.b);
	}

	level->boxes.tab = NULL;

	overlords->tab = NULL;
	overlords->size = 0;

	size_t lv_start_line = colors + 1;
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
					overlord.x = j;
					overlord.y = i - lv_start_line;
					overlord.w = 48;
					overlord.h = 48;
					vect_add(overlord, overlords);
					printf("overlord: %zu, %zu\n", overlord.x, overlord.y);
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
