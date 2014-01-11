
enum {VECTSIZE = 1, VECTGROW = 2};

enum Extensions {
	EXT_BMP,
	EXT_PNG
};

typedef struct Rect Rect;
struct Rect {
	size_t w, h, x, y;
	int from, to;
};

typedef struct Rect_Vect Rect_Vect;
struct Rect_Vect {
	size_t size, max;
	Rect *tab;
};

typedef struct Animation Animation;
struct Animation {
	//dir 1 -right -1 - left
	//fpa - farmes per animation - how often change the frame
	int dir, frame, fpa, fpa_count;
	Rect_Vect clips_r;
};

typedef struct RGB_Color RGB_Color;
struct RGB_Color {
	int r, g, b;
};

void
error(char *msg, ...);

void *
emalloc(size_t size); 

int
ext(const char *file);

void
vect_add(Rect rect, Rect_Vect *vect);

void
vect_print(Rect_Vect *vect);

void
vect_clean(Rect_Vect *vect);

void
vect_del(size_t ind, Rect_Vect *vect);

void
get_last_word(char *source, char *dest);

void
strbefore(char *pice, char *main);
