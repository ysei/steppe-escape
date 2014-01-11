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
int
ext(const char *file) {
	int ext_begin = 0, j = 0;
	char ext[10];
	for (int i = 0; i < strlen(file); i++) {
		if (ext_begin) {
			ext[j++] = file[i];
		} else if (file[i] == '.')
			ext_begin = 1;
	}
	ext[j] = '\0';
	if (strcmp("png", ext) == 0)
		return EXT_PNG;
	else if (strcmp("bmp", ext) == 0)
		return EXT_BMP;
	else
		return -1;
}

void
get_last_word(char *source, char *dest) {
char word[100] = "";
int j = 0;
int i;
for (i = 0; i < strlen(source); i++) {
	if (source[i] == ' ') {
		word[0] = ' ';
		j = 1;
	} else {
		word[j++] = source[i];
	}
}
word[j] = '\0';
source[i - j] = '\0';
strcpy(dest, word);
}
void
strbefore(char *pice, char *main) {
	int p_len;
	p_len = strlen(pice);
	memmove(main + p_len, main, (strlen(main) + 1)*sizeof(char));
	for (int i = 0; i < p_len; i++) {
		main[i] = pice[i];
	}
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
