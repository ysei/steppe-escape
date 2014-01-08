#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../utils.h"

int
main() {
	Rect_Vect rects;
	Rect rect;

	rects.tab = NULL;
	rect.x = 2;
	rect.y = 2;
	rect.w = 2;
	rect.h = 2;
	vect_add(rect, &rects);

	rect.x = 3;
	rect.y = 3;
	rect.w = 3;
	rect.h = 3;
	vect_add(rect, &rects);

	rect.x = 1;
	rect.y = 1;
	rect.w = 1;
	rect.h = 1;
	vect_add(rect, &rects);

	vect_del(2, &rects);
	vect_print(&rects);

	vect_clean(&rects);
	vect_print(&rects);

	return 0;
}
