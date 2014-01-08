
enum {VECTSIZE = 1, VECTGROW = 2};

typedef struct Rect Rect;
struct Rect {
	size_t w, h, x, y;
};

typedef struct Rect_Vect Rect_Vect;
struct Rect_Vect {
	size_t size, max;
	Rect *tab;
};

typedef struct RGB_Color RGB_Color;
struct RGB_Color {
	int r, g, b;
};

void
error(char *msg, ...);

void *
emalloc(size_t size); 

void
vect_add(Rect rect, Rect_Vect *vect);

void
vect_print(Rect_Vect *vect);

void
vect_clean(Rect_Vect *vect);

void
vect_del(size_t ind, Rect_Vect *vect);
