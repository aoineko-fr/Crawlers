// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │    ▄▄                 ▄▄
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │   ██ ▀ ██▄▀ ▄▀██ █ ██ ██  ▄███ ██▄▀  ██▀
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │   ▀█▄▀ ██   ▀▄██ █▀█▀ ▀█▄ ▀█▄▄ ██   ▄██
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
//  Multiplayer snake game
//─────────────────────────────────────────────────────────────────────────────
#pragma once

//=============================================================================
// DEFINES
//=============================================================================

#define GAME_VERSION				"1.1.1"

// Configuration
#define DEF_MUSIC					TRUE
#define DEF_SFX						TRUE
#define PLAYER_MAX					8
#define TRAIN_LEVEL_MAX				40

// Gameplay defines
#define LENGTH_MIN					1
#define LENGTH_MAX					128
#define LENGTH_DEFAULT				5
#define SPAWN_WAIT					20 // Spawn waiting time (N * 8 frames)
#define COOLDOWN_WAIT				8
#define AI_WEIGHT_BONUS				1
#define BONUS_GROWTH				5
#define TRAIN_GROWTH				2
#define BATTLEROYAL_TIME			1

// Logo tiles
#define MSXGL						"[\\]^"
#define CHAR_FIRST					' '
#define LOGO_START					8
#define LOGO_OFFSET					33+16
#define LOGO_END					64+16

// Collider tiles
#define TILE_TREE					0xE1
#define TILE_TREE2					0xE2
#define TILE_HOLE					0xE3
#define TILE_INCOMING				0xE4
#define TILE_FENCE					0xE8
#define TILE_WATER					0xEF
// No-collider tiles
#define TILE_EMPTY					0xF0
#define TILE_PREHOLE				0xF2
#define TILE_BONUS					0xF5
#define TILE_JOY					0xFC // Joystick
#define TILE_KB1					0xFD // Keyboard config 1
#define TILE_KB2					0xFE // Keyboard config 2
#define TILE_HEART					0xFF
// Menu tile
#define TILE_CLOCK					0x03
#define TILE_OK						0x0A
#define TILE_NOT					0x0F
#define TILE_PLAYER					0x3F
#define TILE_BALL					0xE0

// SFX number
#define SFX_MOVE					0
#define SFX_SELECT					6
#define SFX_BACK					4
#define SFX_START					1
#define SFX_BONUS					7
#define SFX_HOLE					14
#define SFX_DEATH					10
#define SFX_VICTORY					11

// Layout defines
#define TRAIN_FRAME_X				13
#define TRAIN_FRAME_Y				8
#define STR_SCROLL_LEN				22
#define COLLAPSE_OFF				0xFF

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
//-----------------------------
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
//-----------------------------
	DIR_MAX,
};

// Input actions define
enum ACTION_TYPE
{
	ACTION_NONE = 0,
	ACTION_RIGHT,
	ACTION_LEFT,
//-----------------------------
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
//-----------------------------
	STATE_MAX,
};

// Menu pages enumeration
enum MENU_PAGE
{
	MENU_MAIN = 0,
	MENU_SOLO,
	MENU_MULTI,
	MENU_OPTION,
	MENU_CREDIT,
	MENU_GRAPH,
	MENU_CONTROL,
	MENU_AUDIO,
	MENU_SAVE,
//-----------------------------
	MENU_MAX,
};

// Game mdoes enumeration
enum GAME_MODE
{
//-- Battle modes
	MODE_GREEDIEST = 0,
	MODE_DEATHMATCH,
	MODE_SIZEMATTERS,
	MODE_BATTLEROYAL,
//-----------------------------
	MODE_BATTLE_MAX,
//-- Solo mode
	MODE_TRAINNNG = MODE_BATTLE_MAX,
//-----------------------------
	MODE_MAX,
};

// Menu pages enumeration
enum MENU_START
{
	START_BATTLE = 0,
	START_TRAIN_NEW,
	START_TRAIN_CONTINUE,
	START_CTRL_TEST,
	START_SCORE_BOARD,
//-----------------------------
	START_MAX,
};

// Frequence enumaration
enum FREQ_MODE
{
	FREQ_AUTO,							// Use auto-detection
	FREQ_60HZ,							// Force 60 Hz
	FREQ_50HZ,							// Force 50 Hz
//-----------------------------
	FREQ_MAX,
};

// Trun mode enumaration
enum TURN_MODE
{
	TURN_RELATIVE,						// All players use relative rotation control
	TURN_ABSOLUTE,						// All players use absolute rotation control
	TURN_CUSTOM,						// Each player can choose their rotation control
//-----------------------------
	TURN_MAX,
};

// Palette enumaration
enum PAL_ID
{
	PAL_CUSTOM = 0,
	PAL_MSX1,
	PAL_MSX2,
	PAL_GRASS,
	PAL_GRAY,
//-----------------------------
	PAL_MAX,
};

// Music enumaration
enum MUSIC_ID
{
	MUSIC_MENU = 0,
	MUSIC_BATTLE,
	MUSIC_HURRYUP,
	MUSIC_VICTORY,
	MUSIC_EMPTY,
//-----------------------------
	MUSIC_MAX,
};

// Speed enumaration
enum SPEED_ID
{
	SPEED_NORMAL = 0,
	SPEED_TURBO,
	SPEED_SNAIL,
	SPEED_CHILL,
//-----------------------------
	SPEED_MAX,
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
	u8 Key;    // Key ID
	u8 Ctrl;   // Controller ID
	bool bAll; // Can apply to all Crawlers
} CtrlBind;

// Speed data structure
typedef struct
{
	u8  Count;
	u16 Score;
} SpeedData;

// Score data structure
typedef struct
{
	u8  Level;
	u16 Score;
	u8  Control;
} ScoreData;

typedef struct
{
	u8			FreqOpt;
	u8			PalOpt;
	bool		OptMusic;
	bool		OptSFX ;
	u8			GameCount;
	u8			BonusOpt;
	u8			WallNum;
	u8			WallOpt;
	u8			Speed;
	u8			TrainLevel;		// Current training level [1~40]
	u8			CtrlTurn;
	u8			HiLevel;
	u16			HiScore[TRAIN_LEVEL_MAX];
	u16			HiTotal;
	u8			BattleController[PLAYER_MAX];	// Players information
	u8			TrainController;	// Players information
	u8			TrainID;	// Players information
} OptionData;

struct PlayerTag;

//
typedef void (*InputCB)(struct PlayerTag* ply);

// Player data structure
typedef struct PlayerTag
{
	u8  ID;
	InputCB Action;
	u8  Controller;
	u8  Turn;
	u8  PosX;
	u8  PosY;
	u8  Dir;
	u8  Length;
	u8  Expect;
	u8  Idx;
	u8  Path[LENGTH_MAX]; // ring buffer
	u8  Anim;
	u8  State;
	u8  Timer;
	u16 Score;
} Player;

// Character data structure
typedef struct
{
	u8        TileBase;
	u8        Sprite;
	u8        FrameX;
	u8        FrameY;
	const u8* Face;
	Vector    EyeOffset;
	u8        EyePattern;
	u8        EyeColor;
} Character;

// Device
typedef struct
{
	const u8*	Default;
	const u8*	Edit;
} SelectDevice;

// Mode description
typedef struct
{
	const c8*	Name;				// Mode's name
	const c8*	Desc;				// Description
	u8			Length;				// Description length
	u8			Count;				// Default round number
	bool		Time;				// Default time
	u8			Bonus;				// Default bonus length
} ModeInfo;

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

#define SELECT_DEV_REL_S		g_DataSelectL31_Names	// 7x2
#define SELECT_DEV_REL			g_DataSelectL32_Names	// 7x2
#define SELECT_DEV_ABS_S		g_DataSelectL33_Names	// 7x2
#define SELECT_DEV_ABS			g_DataSelectL34_Names	// 7x3

#define SELECT_CHARA			g_DataSelectL35_Names	// 7x3

#define SELECT_TEST_NONE		g_DataSelectL36_Names	// 3x3
#define SELECT_TEST_BG			g_DataSelectL37_Names	// 3x3
#define SELECT_TEST_ARROW		g_DataSelectL38_Names	// 4x1

#define SELECT_FACE_1			g_DataFace1L0_Names		// 5x5
#define SELECT_FACE_2			g_DataFace1L1_Names		// 5x5
#define SELECT_FACE_3			g_DataFace1L2_Names		// 5x5
#define SELECT_FACE_4			g_DataFace1L3_Names		// 5x5
#define SELECT_FACE_5			g_DataFace2L0_Names		// 5x5
#define SELECT_FACE_6			g_DataFace2L1_Names		// 5x5
#define SELECT_FACE_7			g_DataFace2L2_Names		// 5x5
#define SELECT_FACE_8			g_DataFace2L3_Names		// 5x5

// Page 0 data
extern const u8 g_DataFace1L0_Names[];
extern const u8 g_DataFace1L1_Names[];
extern const u8 g_DataFace1L2_Names[];
extern const u8 g_DataFace1L3_Names[];
extern const u8 g_DataFace1_Patterns[];
extern const u8 g_DataFace1_Colors[];
extern const u8 g_DataFace2L0_Names[];
extern const u8 g_DataFace2L1_Names[];
extern const u8 g_DataFace2L2_Names[];
extern const u8 g_DataFace2L3_Names[];
extern const u8 g_DataFace2_Patterns[];
extern const u8 g_DataFace2_Colors[];
extern const u8 g_DataLogoSprt[];
extern const u8 g_DataLogoTileL0_Names[];
extern const u8 g_DataLogoTileL1_Names[];
extern const u8 g_DataLogoTile_Patterns[];
extern const u8 g_DataLogoTile_Colors[];
extern const u8 g_MusicEmpty[];
extern const u8 g_MusicGame[];
extern const u8 g_MusicHurry[];
extern const u8 g_MusicIntro[];
extern const u8 g_MusicMain[];
extern const u8 g_MusicVictory[];
extern const u8 g_DataSelectL0_Names[];
extern const u8 g_DataSelectL1_Names[];
extern const u8 g_DataSelectL2_Names[];
extern const u8 g_DataSelectL3_Names[];
extern const u8 g_DataSelectL4_Names[];
extern const u8 g_DataSelectL5_Names[];
extern const u8 g_DataSelectL6_Names[];
extern const u8 g_DataSelectL7_Names[];
extern const u8 g_DataSelectL8_Names[];
extern const u8 g_DataSelectL9_Names[];
extern const u8 g_DataSelectL10_Names[];
extern const u8 g_DataSelectL11_Names[];
extern const u8 g_DataSelectL12_Names[];
extern const u8 g_DataSelectL13_Names[];
extern const u8 g_DataSelectL14_Names[];
extern const u8 g_DataSelectL15_Names[];
extern const u8 g_DataSelectL16_Names[];
extern const u8 g_DataSelectL17_Names[];
extern const u8 g_DataSelectL18_Names[];
extern const u8 g_DataSelectL19_Names[];
extern const u8 g_DataSelectL20_Names[];
extern const u8 g_DataSelectL21_Names[];
extern const u8 g_DataSelectL22_Names[];
extern const u8 g_DataSelectL23_Names[];
extern const u8 g_DataSelectL24_Names[];
extern const u8 g_DataSelectL25_Names[];
extern const u8 g_DataSelectL26_Names[];
extern const u8 g_DataSelectL27_Names[];
extern const u8 g_DataSelectL28_Names[];
extern const u8 g_DataSelectL29_Names[];
extern const u8 g_DataSelectL30_Names[];
extern const u8 g_DataSelectL31_Names[];
extern const u8 g_DataSelectL32_Names[];
extern const u8 g_DataSelectL33_Names[];
extern const u8 g_DataSelectL34_Names[];
extern const u8 g_DataSelectL35_Names[];
extern const u8 g_DataSelectL36_Names[];
extern const u8 g_DataSelectL37_Names[];
extern const u8 g_DataSelectL38_Names[];
extern const u8 g_DataSelect_Patterns[];
extern const u8 g_DataSelect_Colors[];
extern const u8 g_DataSFX[];
extern const u8 g_DataSprites[];
extern const u8 g_DataTiles_Patterns[];
extern const u8 g_DataTiles_Colors[];
extern const u8 g_Level001[];
extern const u8 g_Level002[];
extern const u8 g_Level003[];
extern const u8 g_Level004[];
extern const u8 g_Level005[];
extern const u8 g_Level006[];
extern const u8 g_Level007[];
extern const u8 g_Level008[];
extern const u8 g_Level009[];
extern const u8 g_Level010[];
extern const u8 g_Level011[];
extern const u8 g_Level012[];
extern const u8 g_Level013[];
extern const u8 g_Level014[];
extern const u8 g_Level015[];
extern const u8 g_Level016[];
extern const u8 g_Level017[];
extern const u8 g_Level018[];
extern const u8 g_Level019[];
extern const u8 g_Level020[];
extern const u8 g_Level021[];
extern const u8 g_Level022[];
extern const u8 g_Level023[];
extern const u8 g_Level024[];
extern const u8 g_Level025[];
extern const u8 g_Level026[];
extern const u8 g_Level027[];
extern const u8 g_Level028[];
extern const u8 g_Level029[];
extern const u8 g_Level030[];
extern const u8 g_Level031[];
extern const u8 g_Level032[];
extern const u8 g_Level033[];
extern const u8 g_Level034[];
extern const u8 g_Level035[];
extern const u8 g_Level036[];
extern const u8 g_Level037[];
extern const u8 g_Level038[];
extern const u8 g_Level039[];
extern const u8 g_Level040[];
extern const u16 g_CustomPalette[15];
extern const u16 g_GrassPalette[15];
extern const u16 g_GrayPalette[15];
extern const Shapes g_Body[];
extern const Character g_CharaInfo[PLAYER_MAX];
extern const Start g_Starts[PLAYER_MAX];
extern const u8 g_TitleTile[];
extern const SelectDevice g_DeviceSelect[CTRL_MAX];
extern const SelectDevice g_TurnSelect[2];
extern const SelectSlot g_BattleSelectSlot[10];
extern const u8 g_CursorAnim[8];
extern const u8 g_HoleAnim[4];
extern const CtrlBind g_CtrlBind[14];
extern const u8 g_BonusData[8+1];
extern const u8 g_WallData[4+1];
extern const u16 g_TimerLayout[7];
extern const void* g_MusicInfo[MUSIC_MAX];
extern const u8 g_BallColor[][8];
extern const SpeedData g_SpeedData[SPEED_MAX];
extern const u16 g_ClearBG[];
extern const u8* g_TrainLevelList[];