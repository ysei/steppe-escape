#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>

#include "../utils.h"
#include "../level.h"


/* XPM */
static char * l1_xpm[] = {
"10 10 3 1",
" 	c None",
".	c #000000",
"+	c #00FFFF",
".+..++++..",
".+..++++..",
"....++++..",
"....++++..",
"....++++..",
"....++++..",
"....++++..",
"....++++..",
"....++++..",
"....++++++",
};

int
main() {
	Level level;
	RGB_Color color;

	for (size_t i = 4; i < 14; i++) {
		printf("%zu. %s\n",i - 4, l1_xpm[i]);
	}
	printf("\n");

	load_level(&level, l1_xpm, color, 10);
	for (size_t i = 0; i < level.boxes.size; i++) {
		printf("%zu. x: %zu y:%zu: w: %zu h: %zu\n", i, level.boxes.tab[i].x, level.boxes.tab[i].y, level.boxes.tab[i].w, level.boxes.tab[i].h);
	}
}
