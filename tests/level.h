typedef struct Level Level;
struct Level{
	SDL_Surface *sur;
	Rect_Vect boxes;
};


void
load_level(Level *level, char *xpm[], RGB_Color color, size_t screen_width);
