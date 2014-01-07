#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void
error(char *msg, ...) {
	va_list arg;
	va_start(arg, msg);
	vfprintf(stderr, msg, arg);
	exit(EXIT_FAILURE);
}

void *
emalloc(size_t size) {
	void *p;
	p = malloc(size);
	if (p == NULL) {
		error("emalloc: cannot allocate memory\n");
	}
	return p;
}

void
vect_add(Rect rect, Rect_Vect *vect) {
	Rect *ntab;
	size_t nmax;

	if (vect->tab == NULL) {
		vect->tab = emalloc(VECTSIZE * sizeof(Rect));
		vect->max = VECTSIZE;
		vect->size = 0;
	} else if (vect->size >= vect->max) {
		nmax = vect->max * VECTGROW;
		ntab = realloc(vect->tab, nmax * sizeof(Rect));
		if (ntab == NULL) {
			fprintf(stderr, "vect_add: realloc: cannot grow vector\n");
		} else {
			vect->tab = ntab;
			vect->max = nmax;
		}
	}

	vect->tab[vect->size] = rect;
	vect->size++;
}
