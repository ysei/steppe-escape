#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL.h>

#include "../utils.h"
#include "../level.h"



/* XPM */
/*static char * s_xpm[] = {
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
};*/

int
main() {
	/*Level level;
	RGB_Color color;

	for (size_t i = 5; i < 15; i++) {
		printf("%zu. %s\n",i - 4, s_xpm[i]);
	}
	printf("\n");

	load_level(&level, s_xpm, color, 10);
	vect_print(&level.boxes);*/

	Speeches *speeches[3];

	int lv = 1;

	Speeches *sp = NULL;

	sp = add_speech(11, "Ala ma kota", sp);
	speeches[lv] = sp;
	sp = add_speech(11, "to drugie zdanie", sp);
	sp = add_speech(32, "MONGO", sp);

	long next_speech;
	char speech[300];

	speeches[lv] = load_speech(speech, &next_speech, speeches[lv]);

	printf("%ld\n", next_speech);
	for (int i = 0; i <= 50; i++) {
		printf("%d. ", i);
		while (i == next_speech) {
			printf("%s", speech);
			speeches[lv] = load_speech(speech, &next_speech, speeches[lv]);
			printf("in while: %ld\n", next_speech);
		}
		putchar('\n');
	}
}
