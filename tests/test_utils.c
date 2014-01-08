#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../utils.h"

int
main() {
	Rect_Vect rects;
	Rect rect;

	rects.tab = NULL;
	vect_add(rect, &rects);
	vect_print(&rects);

	vect_clean(&rects);
	vect_print(&rects);

	return 0;
}
