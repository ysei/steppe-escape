typedef struct Plane Plane;
struct Plane {
	int x, true_y, level_y, speed;
	SDL_Surface *sur;
	Rect_Vect boxes;
};

