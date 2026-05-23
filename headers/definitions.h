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

static constexpr int ROWS = 70;

static constexpr int COLS = 70;

static constexpr int LAYERS = 2;

static constexpr int GRID_SIZE = 48;

static constexpr int TILE_SCALE = 3;

static constexpr int ROTATING_SPIKE_MAX_HITBOX = 6;

//these colors can be re used for futures tiles / platforms

//static constexpr Color ONE_WAY_RIGHT = Color{0, 200,190,255};

//static constexpr Color ONE_WAY_LEFT = Color{0, 230,160,255};

