typedef struct Level Level;
struct Level{
	SDL_Surface *sur;
	Rect_Vect boxes;
};

typedef struct Speeches Speeches;
struct Speeches {
	long line;
	char *text;
	Speeches *next;
};


void
load_level(Level *level, char *xpm[], Rect_Vect *overlords, RGB_Color color, size_t screen_width);

Speeches *
load_speech(char *speech, long *line, Speeches *speeches);

Speeches *
add_speech(long line, char *speech, Speeches *last);
