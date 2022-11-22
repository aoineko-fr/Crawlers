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
#define PLAYER_MAX					8

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

#define SPAWN_WAIT					20 // Spawn waiting time (N * 8 frames)
#define COOLDOWN_WAIT				8

#define LOGO_START					8
#define LOGO_OFFSET					33
#define LOGO_END					64


// Controller types define
enum CTRL_TYPE
{
	CTRL_JOY_1 = 0,
	CTRL_JOY_2,
	CTRL_JOY_3,
	CTRL_JOY_4,
	CTRL_JOY_5,
	CTRL_JOY_6,
	CTRL_JOY_7,
	CTRL_JOY_8,
	CTRL_KEY_1,						// 9
	CTRL_KEY_2,
	CTRL_PLY_NUM,
	CTRL_AI_EASY = CTRL_PLY_NUM,	// 11
	CTRL_AI_MED,
	CTRL_AI_HARD,
	CTRL_NONE,						// 14
	CTRL_MAX,
};
#define CTRL_FREE					0xFF
#define CTRL_UNAVAILABLE			0xFE

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
enum ACTION_TYPE
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
	STATE_INIT,
	STATE_COOLDOWN,
	STATE_WAITING,					// Waiting for spawn point to be available
	STATE_SPAWNING,
	STATE_PLAYING,
};

// Menu pages enumeration
enum MENU_PAGE
{
	MENU_MAIN = 0,
	MENU_PLAY,
	MENU_OPTION,
	MENU_CREDIT,
	MENU_MAX,
};

// Game mdoes enumeration
enum GAME_MODE
{
	MODE_BATTLEROYAL = 0,
	MODE_DEATHMATCH,
	MODE_SIZEMATTER,
	MODE_MAX,
};

// Frequence enumaration
enum FREQ_MODE
{
	FREQ_AUTO,							// Use auto-detection
	FREQ_60HZ,							// Force 60 Hz
	FREQ_50HZ,							// Force 50 Hz
	FREQ_MAX,
};

// Start position structure
typedef struct
{
	u8 X;
	u8 Y;
	u8 Dir;
} Start;

// Vector structure
typedef struct
{
	u8 X;
	u8 Y;
} Vector;

// Body shapes structure
typedef struct
{
	u8 A;
	u8 B;
} Shapes;

// Controller binding structure
typedef struct
{
	u8 Key;
	u8 Ctrl;
} CtrlBind;

struct PlayerTag;

//
typedef void (*InputCB)(struct PlayerTag* ply);

// Player data structure
typedef struct PlayerTag
{
	u8 ID;
	InputCB Action;
	u8 Controller;
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
} Player;

// Character data structure
typedef struct
{
	const c8* Name;
	u8        TileBase;
	u8        Sprite;
	u8        FrameX;
	u8        FrameY;
	const u8* Face;
} Character;

// Device
typedef struct
{
	const u8*	Default;
	const u8*	Edit;
} SelectDevice;

// Slot
typedef struct
{
	Vector		UL;
	Vector		DR;
	i8			Left;
	i8			Right;
	i8			Up;
	i8			Down;
} SelectSlot;

// Select menu
#define SELECT_DEV_NONE_S		g_DataSelectL0_Names	// 7x3
#define SELECT_DEV_AI_1_S		g_DataSelectL1_Names	// 7x3
#define SELECT_DEV_AI_2_S		g_DataSelectL2_Names	// 7x3
#define SELECT_DEV_AI_3_S		g_DataSelectL3_Names	// 7x3
#define SELECT_DEV_NONE			g_DataSelectL4_Names	// 7x3
#define SELECT_DEV_AI_1			g_DataSelectL5_Names	// 7x3
#define SELECT_DEV_AI_2			g_DataSelectL6_Names	// 7x3
#define SELECT_DEV_AI_3			g_DataSelectL7_Names	// 7x3
#define SELECT_DEV_JOY_1_S		g_DataSelectL8_Names	// 7x3
#define SELECT_DEV_JOY_2_S		g_DataSelectL9_Names	// 7x3
#define SELECT_DEV_JOY_3_S		g_DataSelectL10_Names	// 7x3
#define SELECT_DEV_JOY_4_S		g_DataSelectL11_Names	// 7x3
#define SELECT_DEV_JOY_1		g_DataSelectL12_Names	// 7x3
#define SELECT_DEV_JOY_2		g_DataSelectL13_Names	// 7x3
#define SELECT_DEV_JOY_3		g_DataSelectL14_Names	// 7x3
#define SELECT_DEV_JOY_4		g_DataSelectL15_Names	// 7x3
#define SELECT_DEV_JOY_5_S		g_DataSelectL16_Names	// 7x3
#define SELECT_DEV_JOY_6_S		g_DataSelectL17_Names	// 7x3
#define SELECT_DEV_JOY_7_S		g_DataSelectL18_Names	// 7x3
#define SELECT_DEV_JOY_8_S		g_DataSelectL19_Names	// 7x3
#define SELECT_DEV_JOY_5		g_DataSelectL20_Names	// 7x3
#define SELECT_DEV_JOY_6		g_DataSelectL21_Names	// 7x3
#define SELECT_DEV_JOY_7		g_DataSelectL22_Names	// 7x3
#define SELECT_DEV_JOY_8		g_DataSelectL23_Names	// 7x3
#define SELECT_DEV_KEYB_1_S		g_DataSelectL24_Names	// 7x3
#define SELECT_DEV_KEYB_2_S		g_DataSelectL25_Names	// 7x3
#define SELECT_DEV_KEYB_1		g_DataSelectL26_Names	// 7x3
#define SELECT_DEV_KEYB_2		g_DataSelectL27_Names	// 7x3

#define SELECT_FRAME			g_DataSelectL28_Names	// 7x6

#define SELECT_START			g_DataSelectL29_Names	// 7x2
#define SELECT_EXIT				g_DataSelectL30_Names	// 6x2

#define SELECT_FACE_1			g_DataFace1L0_Names		// 5x5
#define SELECT_FACE_2			g_DataFace1L1_Names		// 5x5
#define SELECT_FACE_3			g_DataFace1L2_Names		// 5x5
#define SELECT_FACE_4			g_DataFace1L3_Names		// 5x5
#define SELECT_FACE_5			g_DataFace2L0_Names		// 5x5
#define SELECT_FACE_6			g_DataFace2L1_Names		// 5x5
#define SELECT_FACE_7			g_DataFace2L2_Names		// 5x5
#define SELECT_FACE_8			g_DataFace2L3_Names		// 5x5


























