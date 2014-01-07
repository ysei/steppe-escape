#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>

#include "utils.h"
#include "level.h"

void
load_level(Level *level, char *xpm[], RGB_Color color, size_t screen_width) {
	size_t width, height, colors, chars_on_px;
	sscanf(xpm[0], "%zu %zu %zu %zu", &width, &height, &colors, &chars_on_px);
	if (width != screen_width) {
		error("load_level: map width(%d) should be the same as screen_width(%d)\n", width, screen_width);
	} else if (chars_on_px != 1) {
		error("load_level: only one char on pixel allowed in map format\n");
	}
	char wather_ch = '\0';
	for (int i = 1; i <= colors; i++) {
		//I assume here that colors are in format: #XXXXXX
		char ch, code[8];
		sscanf(xpm[i], "%1c %*c %s", &ch, code);
		if (strcmp(code, "#00FFFF")) {
			wather_ch = ch;
		}
	}
	if (wather_ch == '\0') {
		error("load_level: cannot find LEVEL_RIEVR_MASK in mapfile(%d, %d, %d)", color.r, color.g, color.b);
	}

	level->boxes.tab = NULL;
	size_t lv_start_line = colors + 1;
	for (size_t i = lv_start_line; i < width + lv_start_line; i++) {

		size_t w = 0, start_x = 0;
		for (size_t j = 0; j < strlen(xpm[i]); j++) {
			Rect rect;
			if (xpm[i][j] == wather_ch) {
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
			} else {
				w++;
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
