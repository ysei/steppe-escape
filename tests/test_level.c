#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>

#include "../utils.h"
#include "../level.h"

#include "l1.xpm"


/* XPM */
static char * s_xpm[] = {
"10 10 4 1",
" 	c None",
".	c #000000",
"+	c #00FFFF",
"@  c #FF0000",
"....++++..",
"....++++..",
"....++++..",
"....++++..",
"....+@@+..",
"....+@@+..",
"....++++..",
"....++++..",
"....++++..",
"....++++++",
};

int
main() {
	Level level;
	RGB_Color color;

	for (size_t i = 5; i < 15; i++) {
		printf("%zu. %s\n",i - 4, s_xpm[i]);
	}
	printf("\n");

	load_level(&level, s_xpm, color, 10);
	vect_print(&level.boxes);
}
