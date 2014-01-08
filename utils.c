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

void
vect_print(Rect_Vect *vect) {
	printf("size: %zu max: %zu\n", vect->size, vect->max);
	for (size_t i = 0; i < vect->size; i++) {
		printf("%zu. x: %zu y:%zu: w: %zu h: %zu\n", i, vect->tab[i].x, vect->tab[i].y, vect->tab[i].w, vect->tab[i].h);
	}
}

void
vect_clean(Rect_Vect *vect) {
	free(vect->tab);
	vect->tab = NULL;
	vect->size = 0;
	vect->max = 0;
}

void
vect_del(size_t ind, Rect_Vect *vect) {
	if (ind < vect->size) {
		if (memmove(&vect->tab[ind], &vect->tab[ind+1], (vect->size-1 - ind) * sizeof(Rect)) == NULL) {
			error("vect_del: Cannot delete elm from vector\n");
		} else {
			vect->size--;
		}
	}
}
