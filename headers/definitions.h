#pragma once

#include "raylib.h"

static constexpr Color PLAYER_SPAWN = Color{251,242,54,255};

static constexpr Color GOAL = Color{0,255,0,255};

static constexpr Color TRAMPOLINE = Color{150,150,200,255};

static constexpr Color GRAVITY_CHANGER = Color{150,200,200,255};

static constexpr Color TREADMILL_RIGHT = Color{0,0,255,255};

static constexpr Color TREADMILL_LEFT = Color{0,0,155,255};

static constexpr Color ONE_WAY = Color{0, 255,255,255};

static constexpr Color SPIKE = Color{220,45,50,255};

static constexpr Color DECO = Color{135,30,255,255};

//platforms

static constexpr Color PLATFORM_STOP = Color{255,100,255,255};

static constexpr Color HORIZONTAL_MOVING_PLATFORM = Color{0,100,255,255};

static constexpr Color VERTICAL_MOVING_PLATFORM = Color{255,100,0,255};

static constexpr Color FALLING_PLATFORM = Color{200,100,0,255};

static constexpr Color DISAPPEARING_PLATFORM = Color{200,100,140,255};

static constexpr Color LADDER = Color{0, 200,255,255};

static constexpr Color ENEMY_DUMMY = Color{217,160,102,255};

static constexpr Color ENEMY_YUUKA = Color{85,56,171,255};


//grid

static constexpr int TILE_SIZE = 12;

static const int NATIVE_WIDTH = 240;

static const int NATIVE_HEIGHT = 240;

static const int UI_HEIGHT = TILE_SIZE * 4;

//canvas
static constexpr int CANVAS_WIDTH = NATIVE_WIDTH;

static constexpr int GAMEPLAY_CANVAS_HEIGHT = (NATIVE_HEIGHT - UI_HEIGHT);

static constexpr int UI_CANVAS_HEIGHT = UI_HEIGHT;

//rooms
static constexpr int TILES_PER_ROOM_WIDHT = CANVAS_WIDTH / TILE_SIZE;

static constexpr int TILES_PER_ROOM_HEIGHT = GAMEPLAY_CANVAS_HEIGHT / TILE_SIZE;


static constexpr int ROWS = TILES_PER_ROOM_HEIGHT * 10;

static constexpr int COLS = TILES_PER_ROOM_WIDHT * 10;


static constexpr int LAYERS = 4;

static constexpr int BACKGROUND_LAYER = 0;

static constexpr int GAMEPLAY_LAYER_START = 1;

static constexpr int GAMEPLAY_LAYER_END = 2;

static constexpr int FOREGROUND_LAYER = 3;

static constexpr int SINGLE_ROTATING_SPIKE_MAX_HITBOX = 6; //6

static constexpr int DOUBLE_ROTATING_SPIKE_MAX_HITBOX = 4;

static constexpr float GRAVITY = 875.0f;


static constexpr int MAX_PALETTE_ROWS = 8; //number of squares

static constexpr int MAX_PALETTE_COLS = 4; //number of squares

//GBC COLORS

static constexpr Color GBC_WHITE = Color{255,255,255,255};

static constexpr Color GBC_LIGHT_GRAY = Color{165,165,165,255};

static constexpr Color GBC_DARK_GRAY = Color{82,82,82,255};

static constexpr Color GBC_BLACK = Color{0,0,0,255};


static constexpr Color GBC_DARK_VIOLET = Color{91,49,91,255};

static constexpr Color GBC_RED = Color{230,0,0,255};

static constexpr Color GBC_MAGENTA = Color{255,0,254,255};

static constexpr Color GBC_SKY_BLUE = Color{100,165,255,255};

static constexpr Color GBC_PALE_YELLOW = Color{255,255,165,255};

static constexpr Color GBC_DARKEST_BROWN = Color{58,40,2,255};

static constexpr Color GBC_DARK_BROWN = Color{131,49,0,255};

static constexpr Color GBC_BROWN = Color{165,107,49,255};

static constexpr Color GBC_DULL_TURQUOISE = Color{101,164,155,255};