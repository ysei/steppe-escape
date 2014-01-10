//Screen dimension constants 
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

//Game Scale: 1px = 0,21559m
//River Length: 525km = 525000m = 2435177px
//KWS1: Rozpietosc: 9,04m Dlugosc: 7,33m(34px) Wysokosc: 2,7m
const double PIXEL_SCALE = 0.21559;

const int MAX_PATH_LEN = 100;

const char IMAGE_PATH[] = "images";

const char LEVEL_PATH[] = "levels";
const char LEVEL_PREFIX[] = "l";
const RGB_Color LEVEL_RIVER_MASK = {0, 0xFF, 0xFF};

const int FRAMES_PER_SECOND = 50;

const int PLANE_X_SPEED = 10;

const int PLANE_START_SPEED = 3;
const int PLANE_SPEED_MAX = 5;
const int PLANE_SPEED_MIN = 1;
const int PLANE_SPEED_MULTIPLAYER = 1;

const int BULLET_SPEED = 20;
//Mustafar LK Theorotical Rate: 250 (shoots/min) 
const int THEORETICAL_RATE = 250;

const char FONT_PATH[] = "fonts";

const char SOUND_PATH[] = "sounds";

enum Surfaces {
	SUR_START,
	SUR_PLANE,
	SUR_OVERLORD,
	SUR_TOTAL,
};
const char *Images[] = {"start.bmp", "kws1.bmp", "overlord.bmp"};

enum Levels {
	LEVEL_1,
	LEVEL_TOTAL,
};

enum Font {
	FONT_INFO,
	FONT_MENU,
	FONT_TOTAL,
};
const char *Fonts[] = {"DejaVuSans.ttf", "badabb.ttf"};
const int Fonts_Sizes[] = {12, 48};

enum Sound {
	SOUND_GUN,
	SOUND_OV_DEATH,
	SOUND_ENGINE,
	SOUND_BIG_EXPLOSION,
	SOUND_TOTAL,
};
const char *Sounds[] = {"gun_single.ogg", "ov_death.ogg", "machine1.ogg", "big_explosion.ogg"};
