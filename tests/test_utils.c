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
	return 0;
}
