// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │    ▄▄                 ▄▄
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │   ██ ▀ ██▄▀ ▄▀██ █ ██ ██  ▄███ ██▄▀  ██▀
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │   ▀█▄▀ ██   ▀▄██ █▀█▀ ▀█▄ ▀█▄▄ ██   ▄██
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
//  Multiplayer snake game
//─────────────────────────────────────────────────────────────────────────────

//=============================================================================
// DEFINES
//=============================================================================

// Library's logo
#define MSX_GL						"\x02\x03\x04\x05"

// 
#define CHAR_FIRST					' '

//
#define LENGTH_MIN					1
#define LENGTH_MAX					128
#define LENGTH_DEFAULT				5

// Collider tiles
#define TILE_TREE					0xE0
#define TILE_TREE2					0xE1
#define TILE_EGG					0xE2
#define TILE_HOLE					0xE3
#define TILE_INCOMING				0xE4
// No-collider tiles
#define TILE_EMPTY					0xF0
#define TILE_SALAD					0xF1
#define TILE_MUSH					0xF2
#define TILE_PREHOLE				0xF3

#define SPAWN_WAIT					16 // Spawn waiting time (N * 8 frames)

// Input types define
enum INPUT_TYPE
{
	INPUT_JOY_1 = 0,
	INPUT_JOY_2,
	INPUT_JOY_3,
	INPUT_JOY_4,
	INPUT_JOY_5,
	INPUT_JOY_6,
	INPUT_JOY_7,
	INPUT_JOY_8,
	INPUT_KEY_1,					// 9
	INPUT_KEY_2,
	INPUT_NUM,
	INPUT_AI_EASY = INPUT_NUM,		// 11
	INPUT_AI_MED,
	INPUT_AI_HARD,
	INPUT_NONE,						// 14
};

// Direction define
enum DIRECTION
{
	DIR_UP = 0, // 00
	DIR_RIGHT,  // 01
	DIR_DOWN,   // 10
	DIR_LEFT,   // 11
	DIR_MAX,
};

// Input actions define
enum INPUT_ACTION
{
	ACTION_NONE = 0,
	ACTION_RIGHT,
	ACTION_LEFT,
	ACTION_MAX,
};

// Player state define
enum PLAYER_STATE
{
	STATE_NONE = 0,
	STATE_WAITING,					// Waiting for spawn point to be available
	STATE_SPAWNING,
	STATE_PLAYING,
};

// Start position structure
struct Start
{
	u8 X;
	u8 Y;
	u8 Dir;
};

// Vector structure
struct Vector
{
	u8 X;
	u8 Y;
};

// Body shapes structure
struct Shapes
{
	u8 A;
	u8 B;
};

struct Player;

//
typedef void (*InputCB)(struct Player* ply);

// Player data structure
struct Player
{
	u8 ID;
	InputCB Action;
	u8 Input;
	u8 PosX;
	u8 PosY;
	u8 Dir;
	u8 Length;
	u8 Expect;
	u8 Idx;
	u8 Path[LENGTH_MAX]; // ring buffer
	u8 Anim;
	u8 State;
	u8 Timer;
	u8 Score;
};

// Character data structure
struct Character
{
	const c8* Name;
	u8 TileBase;
	u8 Sprite;
};
