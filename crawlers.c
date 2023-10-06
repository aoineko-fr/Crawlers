// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │    ▄▄                 ▄▄
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │   ██ ▀ ██▄▀ ▄▀██ █ ██ ██  ▄███ ██▄▀  ██▀
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │   ▀█▄▀ ██   ▀▄██ █▀█▀ ▀█▄ ▀█▄▄ ██   ▄██
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
//  Multiplayer snake game
//─────────────────────────────────────────────────────────────────────────────

//=============================================================================
// INCLUDES
//=============================================================================

// MSXgl
#include "msxgl.h"
#include "memory.h"
#include "string.h"
#include "fsm.h"
#include "game_menu.h"
#include "device/ninjatap.h"
#include "arkos/akg_player.h"
#include "compress/pletter.h"
#include "dos.h"

// Crawlers
#include "crawlers.h"

//=============================================================================
// DEFINES
//=============================================================================

// Prototypes
void State_Init_Begin();
void State_Init_Update();

void State_Logo_Begin();
void State_Logo_Update();

void State_Title_Begin();
void State_Title_Update();

void State_Menu_Begin();
void State_Menu_Update();

void State_BattleSelect_Begin();
void State_BattleSelect_Update();

void State_BattleStart_Begin();
void State_BattleStart_Update();

void State_BattleGame_Begin();
void State_BattleGame_Update();

void State_Victory_Begin();
void State_Victory_Update();

void State_TrainSelect_Begin();
void State_TrainSelect_Update();

void State_TrainGame_Begin();
void State_TrainGame_Update();

void State_TrainScore_Begin();
void State_TrainScore_Update();

void MenuOpen_Multi();
void MenuOpen_Solo();
void MenuOpen_Credit();

const c8* MenuAction_Start(u8 op, i8 value);
const c8* MenuAction_Mode(u8 op, i8 value);
const c8* MenuAction_Speed(u8 op, i8 value);
const c8* MenuAction_Info(u8 op, i8 value);
const c8* MenuAction_Credits(u8 op, i8 value);
const c8* MenuAction_Freq(u8 op, i8 value);
const c8* MenuAction_Palette(u8 op, i8 value);
const c8* MenuAction_Music(u8 op, i8 value);
const c8* MenuAction_SFX(u8 op, i8 value);
const c8* MenuAction_Bonus(u8 op, i8 value);
const c8* MenuAction_Wall(u8 op, i8 value);
const c8* MenuAction_Port(u8 op, i8 value);
const c8* MenuAction_MSX(u8 op, i8 value);
const c8* MenuAction_VDP(u8 op, i8 value);
// const c8* MenuAction_Save(u8 op, i8 value);
const c8* MenuAction_Turn(u8 op, i8 value);

void InitPlayer(Player* ply, u8 id);
void ResetPlayer(Player* ply);
void SpawnPlayer(Player* ply);
void DrawPlayer(Player* ply, u8 x, u8 y);
void ClearPlayer(Player* ply);
void UpdatePlayer(Player* ply);

bool IsInputUp();
bool IsInputDown();
bool IsInputLeft();
bool IsInputRight();
bool IsInputButton1();
bool IsInputButton2();

#if (TARGET_TYPE == TYPE_DOS)
u8 g_VersionROM = 0;
u8 g_VersionMSX = 0;
#else
extern u8 g_VersionROM;
extern u8 g_VersionMSX;
#endif

extern MenuItemMinMax g_MenuLevelMinMax;

//=============================================================================
// READ-ONLY DATA
//=============================================================================

#if (TARGET_TYPE == TYPE_DOS)
	#include "crawlers_p0.c"
#endif

//
const Shapes g_Body[] =
{
	{ 0x46, 0x47 }, // U+U =>	00:00
	{ 0x48, 0x48 }, // U+R =>	00:02
	{ 0,    0,   }, // U+D =>	00:01
	{ 0x49, 0x49 }, // U+L =>	00:03

	{ 0x4B, 0x4B }, // R+U =>	02:00
	{ 0x44, 0x45 }, // R+R =>	02:02
	{ 0x49, 0x49 }, // R+D =>	02:01
	{ 0,    0,   }, // R+L =>	02:03

	{ 0,    0,   }, // D+U =>	01:00
	{ 0x4A, 0x4A }, // D+R =>	01:02
	{ 0x46, 0x47 }, // D+D =>	01:01
	{ 0x4B, 0x4B }, // D+L =>	01:03

	{ 0x4A, 0x4A }, // L+U =>	02:00
	{ 0,    0,   }, // L+R =>	02:02
	{ 0x48, 0x48 }, // L+D =>	02:01
	{ 0x44, 0x45 }, // L+L =>	02:03
};

//
const Character g_CharaInfo[PLAYER_MAX] =
{
	{ 0*20, 0   , 2,    5,  SELECT_FACE_1, { 33, 19 }, 12, COLOR_DARK_BLUE },
	{ 1*20, 1   , 2+7,  5,  SELECT_FACE_2, { 30, 16 }, 13, COLOR_DARK_BLUE },
	{ 2*20, 0xFF, 2+14, 5,  SELECT_FACE_3, { 30, 15 }, 14, COLOR_BLACK },
	{ 3*20, 0xFF, 2+21, 5,  SELECT_FACE_4, { 30, 17 }, 15, COLOR_BLACK },
	{ 4*20, 0xFF, 2,    15, SELECT_FACE_5, { 30, 15 }, 14, COLOR_BLACK },
	{ 5*20, 0xFF, 2+7,  15, SELECT_FACE_6, { 30, 17 }, 15, COLOR_DARK_BLUE },
	{ 6*20, 2   , 2+14, 15, SELECT_FACE_7, { 33, 19 }, 12, COLOR_DARK_BLUE },
	{ 7*20, 3   , 2+21, 15, SELECT_FACE_8, { 30, 16 }, 13, COLOR_BLACK },
};

//
const Start g_Starts[PLAYER_MAX] =
{
	{  5,  6, DIR_RIGHT },
	{ 26, 18, DIR_LEFT  },
	{ 26,  6, DIR_DOWN  },
	{  5, 18, DIR_UP    },
	{  9, 10, DIR_DOWN  },
	{ 22, 16, DIR_UP    },
	{  9, 16, DIR_RIGHT },
	{ 22, 10, DIR_LEFT  },
};

// Alternative
const u16 g_MSX2Palette[15] =
{
	RGB16(0, 0, 0), // black				RGB16(0, 0, 0),
	RGB16(1, 5, 1), // medium green			RGB16(1, 5, 1),
	RGB16(3, 6, 3), // light green			RGB16(3, 6, 3),
	RGB16(2, 2, 6), // dark blue			RGB16(2, 2, 6),
	RGB16(3, 3, 7), // light blue			RGB16(3, 3, 7),
	RGB16(5, 2, 2), // dark red				RGB16(5, 2, 2),
	RGB16(5, 5, 7), // *cyan				RGB16(2, 6, 7),
	RGB16(6, 3, 3), // *medium red			RGB16(6, 2, 2),
	RGB16(7, 4, 4), // *light red			RGB16(6, 3, 3),
	RGB16(5, 5, 3), // *dark yellow			RGB16(5, 5, 2),
	RGB16(6, 6, 4), // *light yellow		RGB16(6, 6, 3),
	RGB16(1, 4, 1), // dark green			RGB16(1, 4, 1),
	RGB16(5, 2, 4), // *magenta				RGB16(5, 2, 5),
	RGB16(5, 5, 5), // gray					RGB16(5, 5, 5),
	RGB16(7, 7, 7)  // white				RGB16(7, 7, 7) 
};

const u16 g_GrayPalette[15] =
{
	RGB16(0, 0, 0), // black				RGB16(0, 0, 0),
	RGB16(3, 3, 3), // medium green			RGB16(1, 5, 1),
	RGB16(6, 6, 6), // light green			RGB16(3, 6, 3),
	RGB16(3, 3, 3), // dark blue			RGB16(2, 2, 6),
	RGB16(4, 4, 4), // light blue			RGB16(3, 3, 7),
	RGB16(2, 2, 2), // dark red				RGB16(5, 2, 2),
	RGB16(6, 6, 6), // *cyan				RGB16(2, 6, 7),
	RGB16(3, 3, 3), // *medium red			RGB16(6, 2, 2),
	RGB16(4, 4, 4), // *light red			RGB16(6, 3, 3),
	RGB16(4, 4, 4), // *dark yellow			RGB16(5, 5, 2),
	RGB16(5, 5, 5), // *light yellow		RGB16(6, 6, 3),
	RGB16(2, 2, 2), // dark green			RGB16(1, 4, 1),
	RGB16(4, 4, 4), // *magenta				RGB16(5, 2, 5),
	RGB16(4, 4, 4), // gray					RGB16(5, 5, 5),
	RGB16(7, 7, 7)  // white				RGB16(7, 7, 7) 
};

//
const u8 g_TitleTile[] =
{
	0x70, 0x6D, 0x71, 0xD4, 0xD0, 0xD5, 0x48, 0x45, 0x49, 0x8D, 0x00, 0x7C, 0xA1, 0x00, 0x00, 0xC0, 0xBD, 0xCA, 0xAC, 0xA8, 0xAD, 0x5C, 0x58, 0x55, 
	0x6F, 0x00, 0x6A, 0xD2, 0xCF, 0xD7, 0x47, 0x00, 0x42, 0x83, 0x00, 0x83, 0x97, 0x00, 0x00, 0xBA, 0x00, 0x00, 0xAA, 0xA7, 0xAF, 0x5A, 0x00, 0x00, 
	0x6E, 0x00, 0x00, 0xD3, 0xCF, 0xD5, 0x46, 0x43, 0x49, 0x82, 0x00, 0x82, 0x96, 0x00, 0x00, 0xC0, 0xB9, 0x00, 0xAB, 0xA7, 0xAD, 0x5E, 0x59, 0x5D, 
	0x6F, 0x00, 0x79, 0xD2, 0x00, 0xD3, 0x47, 0x00, 0x47, 0x83, 0x7C, 0x83, 0x97, 0x00, 0x00, 0xBF, 0x00, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0x00, 0x5B, 
	0x72, 0x6C, 0x73, 0xD9, 0x00, 0xD8, 0x4D, 0x00, 0x4C, 0x86, 0x87, 0x89, 0x9A, 0x94, 0x91, 0xC2, 0xBC, 0xCB, 0xB0, 0x00, 0xB1, 0x63, 0x58, 0x5F, 
};

const MenuItemMinMax g_MenuRoundsMinMax = { 1, 20, 1 };
const MenuItemMinMax g_MenuTreesMinMax =  { 0, 100, 10 };

// MENU_MAIN - Main menu
const MenuItem g_MenuMain[] =
{
	{ "BATTLE",              MENU_ITEM_GOTO, NULL, MENU_MULTI },
	{ "TRAINING",            MENU_ITEM_GOTO, NULL, MENU_SOLO },
	{ "OPTIONS",             MENU_ITEM_GOTO, NULL, MENU_OPTION },
	{ "CREDITS",             MENU_ITEM_GOTO, NULL, MENU_CREDIT },
};

// MENU_SOLO - Training menu
MenuItem g_MenuSolo[] =
{
	{ "NEW GAME",            MENU_ITEM_ACTION, MenuAction_Start, START_TRAIN_NEW },
	{ "CONTINUE",            MENU_ITEM_ACTION, MenuAction_Start, START_TRAIN_CONTINUE },
	{ "LEVEL",               MENU_ITEM_INT, &g_TrainLevel, (i16)&g_MenuLevelMinMax },
	{ "SPEED",               MENU_ITEM_ACTION, MenuAction_Speed, 0 },
	// { "SCORE",               MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_TrainTotal, NULL },
	// { "HI-SCORE",            MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_HiTotal, NULL },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

// MENU_MULTI - Multiplayer battle menu
const MenuItem g_MenuMulti[] =
{
	{ "START",               MENU_ITEM_ACTION, MenuAction_Start, START_BATTLE },
	{ "MODE",                MENU_ITEM_ACTION, MenuAction_Mode, 0 },
	{ "COUNT",               MENU_ITEM_INT, &g_GameCount, (i16)&g_MenuRoundsMinMax },
	{ "WALLS",               MENU_ITEM_INT, &g_WallNum, (i16)&g_MenuTreesMinMax },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_UPDATE, MenuAction_Info, -1 },
};

// MENU_OPTION - Options menu
const MenuItem g_MenuOption[] =
{
	{ "GRAPH",               MENU_ITEM_GOTO, NULL, MENU_GRAPH },
	{ "CONTROL",             MENU_ITEM_GOTO, NULL, MENU_CONTROL },
	{ "AUDIO",               MENU_ITEM_GOTO, NULL, MENU_AUDIO },
	// { "SAVE",                MENU_ITEM_ACTION, MenuAction_Save, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

// MENU_GRAPH - Graphic options menu
const MenuItem g_MenuGraph[] =
{
	{ "SYSTEM",              MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_MSX, 0 },
	{ "VIDEO",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_VDP, 0 },
	{ "FREQ",                MENU_ITEM_ACTION, MenuAction_Freq, 0 },
	{ "PALETTE",             MENU_ITEM_ACTION, MenuAction_Palette, 0 },
	{ "BONUS",               MENU_ITEM_ACTION, MenuAction_Bonus, 0 },
	{ "WALL",                MENU_ITEM_ACTION, MenuAction_Wall, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_OPTION },
};

// MENU_CONTROL - Control options menu
const MenuItem g_MenuControl[] =
{
	{ "PORT1",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_Port, 0 },
	{ "PORT2",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_Port, 1 },
	{ "MAX JOY",             MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_JoyNum, NULL },
	{ "MAX PLY",             MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_PlayerMax, NULL },
	{ "TURN",                MENU_ITEM_ACTION, MenuAction_Turn, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_OPTION },
};

// MENU_AUDIO - Audio options menu
const MenuItem g_MenuAudio[] =
{
	{ "MUSIC",               MENU_ITEM_ACTION, MenuAction_Music, 0 },
	{ "TEST",                MENU_ITEM_ACTION, MenuAction_Music, 1 },
	{ "SFX",                 MENU_ITEM_ACTION, MenuAction_SFX, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_OPTION },
};

// MENU_CREDIT - Credit page
const MenuItem g_MenuCredit[] =
{
	{ "CODE   AOINEKO",      MENU_ITEM_TEXT, NULL, 0 },
	{ "MUSIC  TOTTA",        MENU_ITEM_TEXT, NULL, 0 },
	{ "SFX    AOINEKO,TOTTA",MENU_ITEM_TEXT, NULL, 0 },
	{ "GRAPH  AOINEKO,GFX",  MENU_ITEM_TEXT, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_UPDATE, MenuAction_Credits, -1 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

// List of all menus
const Menu g_Menus[MENU_MAX] =
{
	{ NULL, g_MenuMain,    numberof(g_MenuMain),    NULL },				// MENU_MAIN
	{ NULL, g_MenuSolo,    numberof(g_MenuSolo),    MenuOpen_Solo },	// MENU_SOLO
	{ NULL, g_MenuMulti,   numberof(g_MenuMulti),   MenuOpen_Multi },	// MENU_MULTI
	{ NULL, g_MenuOption,  numberof(g_MenuOption),  NULL },				// MENU_OPTION
	{ NULL, g_MenuCredit,  numberof(g_MenuCredit),  MenuOpen_Credit },	// MENU_CREDIT
	{ NULL, g_MenuGraph,   numberof(g_MenuGraph),   NULL },				// MENU_GRAPH
	{ NULL, g_MenuControl, numberof(g_MenuControl), NULL },				// MENU_CONTROL
	{ NULL, g_MenuAudio,   numberof(g_MenuAudio),   NULL },				// MENU_AUDIO
};

// List of all states
const FSM_State State_Init =			{ 0, State_Init_Begin,			State_Init_Update,			NULL };
const FSM_State State_Logo =			{ 0, State_Logo_Begin,			State_Logo_Update,			NULL };
const FSM_State State_Title =			{ 0, State_Title_Begin,			State_Title_Update,			NULL };
const FSM_State State_Menu =			{ 0, State_Menu_Begin,			State_Menu_Update,			NULL };
const FSM_State State_BattleSelect =	{ 0, State_BattleSelect_Begin,	State_BattleSelect_Update,	NULL };
const FSM_State State_BattleStart =		{ 0, State_BattleStart_Begin,	State_BattleStart_Update,	NULL };
const FSM_State State_BattleGame =		{ 0, State_BattleGame_Begin,	State_BattleGame_Update,	NULL };
const FSM_State State_Victory =			{ 0, State_Victory_Begin,		State_Victory_Update,		NULL };
const FSM_State State_TrainSelect =		{ 0, State_TrainSelect_Begin,	State_TrainSelect_Update,	NULL };
const FSM_State State_TrainGame =		{ 0, State_TrainGame_Begin,		State_TrainGame_Update,		NULL };
const FSM_State State_TrainScore =		{ 0, State_TrainScore_Begin,	State_TrainScore_Update,	NULL };

// 
const SelectDevice g_DeviceSelect[CTRL_MAX] =
{
	{ SELECT_DEV_JOY_1,  SELECT_DEV_JOY_1_S },
	{ SELECT_DEV_JOY_2,  SELECT_DEV_JOY_2_S },
	{ SELECT_DEV_JOY_3,  SELECT_DEV_JOY_3_S },
	{ SELECT_DEV_JOY_4,  SELECT_DEV_JOY_4_S },
	{ SELECT_DEV_JOY_5,  SELECT_DEV_JOY_5_S },
	{ SELECT_DEV_JOY_6,  SELECT_DEV_JOY_6_S },
	{ SELECT_DEV_JOY_7,  SELECT_DEV_JOY_7_S },
	{ SELECT_DEV_JOY_8,  SELECT_DEV_JOY_8_S },
	{ SELECT_DEV_KEYB_1, SELECT_DEV_KEYB_1_S },
	{ SELECT_DEV_KEYB_2, SELECT_DEV_KEYB_2_S },
	{ SELECT_DEV_AI_1,   SELECT_DEV_AI_1_S },
	{ SELECT_DEV_AI_2,   SELECT_DEV_AI_2_S },
	{ SELECT_DEV_AI_3,   SELECT_DEV_AI_3_S },
	{ SELECT_DEV_NONE,   SELECT_DEV_NONE_S },
};

// 
const SelectDevice g_TurnSelect[2] =
{
	{ SELECT_DEV_REL, SELECT_DEV_REL_S },
	{ SELECT_DEV_ABS, SELECT_DEV_ABS_S },
};

// Battle select slots data
const SelectSlot g_BattleSelectSlot[] =
{	//                             L   R   U   D
	{ {  22,  53-8 }, {  58,  89-8 }, -1,  1,  8,  4 }, // 0 - Face 1
	{ {  78,  53-8 }, { 114,  89-8 },  0,  2,  8,  5 }, // 1 - Face 2
	{ { 134,  53-8 }, { 170,  89-8 },  1,  3,  9,  6 }, // 2 - Face 3
	{ { 190,  53-8 }, { 226,  89-8 },  2, -1,  9,  7 }, // 3 - Face 4
	{ {  22, 125 }, {  58, 161 }, -1,  5,  0, -1 }, // 4 - Face 5
	{ {  78, 125 }, { 114, 161 },  4,  6,  1, -1 }, // 5 - Face 6
	{ { 134, 125 }, { 170, 161 },  5,  7,  2, -1 }, // 6 - Face 7
	{ { 190, 125 }, { 226, 161 },  6, -1,  3, -1 }, // 7 - Face 8
	{ {  22,  15-8 }, {  58,  22-8 }, -1,  9, -1,  0 }, // 8 - Start
	{ { 198,  15-8 }, { 226,  22-8 },  8, -1, -1,  3 }, // 9 - Exit
};

// Cursor offset animation
const u8 g_CursorAnim[8] = { 1, 1, 2, 1, 1, 0, 0, 0 };

// Hole tile animation
const u8 g_HoleAnim[4] = { TILE_PREHOLE, (u8)(TILE_PREHOLE+1), (u8)(TILE_PREHOLE+2), TILE_HOLE };

// Controller binding
const CtrlBind g_CtrlBind[] =
{
	{ KEY_DEL,	CTRL_NONE,    TRUE },
	{ KEY_F1,	CTRL_AI_EASY, TRUE },
	{ KEY_F2,	CTRL_AI_MED,  TRUE },
	{ KEY_F3,	CTRL_AI_HARD, TRUE },
	{ KEY_F4,	CTRL_KEY_1,   FALSE },
	{ KEY_F5,	CTRL_KEY_2,   FALSE },
	{ KEY_1,	CTRL_JOY_1,   FALSE },
	{ KEY_2,	CTRL_JOY_2,   FALSE },
	{ KEY_3,	CTRL_JOY_3,   FALSE },
	{ KEY_4,	CTRL_JOY_4,   FALSE },
	{ KEY_5,	CTRL_JOY_5,   FALSE },
	{ KEY_6,	CTRL_JOY_6,   FALSE },
	{ KEY_7,	CTRL_JOY_7,   FALSE },
	{ KEY_8,	CTRL_JOY_8,   FALSE },
};

const u8 g_BonusData[8+1] = { 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFF, 0 };
const u8 g_WallData[4+1] = { TILE_TREE, TILE_TREE2, TILE_FENCE, TILE_WATER, 0 };

const c8 g_DescGreediest[]   = "        GREEDIEST: THE ONE WHO GET A BONUS WINS A POINT. THE FIRST TO REACH THE CHOSEN NUMBER OF BONUSES (COUNT) WINS THE BATTLE.";
const c8 g_DescDeathMatch[]  = "        DEATH MATCH: THE ONE WHO ELIMINATES AN OPPONENT WINS A POINT. THE FIRST TO REACH THE CHOSEN NUMBER OF KILLS (COUNT) WINS THE BATTLE.";
const c8 g_DescSizeMatter[]  = "        SIZE MATTER: AT THE END OF THE CHOSEN NUMBER OF MINUTES (COUNT), THE ONE WHO HAS REACHED THE LARGEST SIZE WINS THE BATTLE.";
const c8 g_DescBattleRoyal[] = "        BATTLE ROYAL: THE LAST SURVIVOR WINS A ROUND. THE MATCH ENDS WHEN THE CHOSEN NUMBER OF ROUNDS (COUNT) IS REACHED. FIELD COLLAPSE AFTER TIMER ENDS.";

const ModeInfo g_ModeInfo[] =
{//   Name            Dec.               Length                     Rnd Time   Length
	{ "GREEDIEST",    g_DescGreediest,   sizeof(g_DescGreediest),   10, FALSE, 5 },
	{ "DEATH MATCH",  g_DescDeathMatch,  sizeof(g_DescDeathMatch),  5,  FALSE, 5 },
	{ "SIZE MATTER",  g_DescSizeMatter,  sizeof(g_DescSizeMatter),  2,  TRUE,  5 },
	{ "BATTLE ROYAL", g_DescBattleRoyal, sizeof(g_DescBattleRoyal), 3,  TRUE,  5 },
};

const c8 g_TextCredits[]   = "        CRAWLERS BY PIXEL PHENIX 2023    VERSION " GAME_VERSION "    POWERED BY [\\]^    DESIGN, CODE AND GFX BY GUILLAUME 'AOINEKO' BLANCHARD    MUSIC AND SFX BY TOTTA    GFX ENHANCEMENT BY LUDOVIC 'GFX' AVOT    FONT BY DAMIEN GUARD    THANKS TO ALL MRC, MSX VILLAGE AND [\\]^ DISCORD MEMBERS FOR SUPPORT    MSX STILL ALIVE!!    DEDICATED TO MY WONDERFUL WIFE AND SON \x1F\x1F  ";

// 14 13 12  |  OOO  O    OO   OO    O    OO   O
// 11 10 09  |  O    O      O    O  OO   O    O O
// 08 07 06  |  OO   OOO  OO    O    O   O    O O
// 05 04 03  |    O   O     O  O     O   O O  O O
// 02 01 00  |  OO    O   OO   OOO  OOO   OO   O
const u16 g_TimerLayout[7] =
{
	0b111100110001110, // 5
	0b100100111010010, // 4
	0b110001110001110, // 3
	0b110001010100111, // 2
	0b010110010010111, // 1
	0b011100100101011, // G
	0b010101101101010, // O
};

//
const void* g_MusicInfo[MUSIC_MAX] =
{
	{ g_MusicIntro   }, // MUSIC_MENU
	{ g_MusicGame    }, // MUSIC_BATTLE
	{ g_MusicHurry   }, // MUSIC_HURRYUP
	{ g_MusicVictory }, // MUSIC_VICTORY
	{ g_MusicEmpty   }, // MUSIC_EMPTY
};

//
const u8 g_BallColor[][8] =
{
	{ 0xB5, 0xB5, 0x75, 0x55, 0x55, 0x54, 0xA5, 0xA4 }, // Blue
	{ 0xB2, 0xB2, 0x32, 0x22, 0x22, 0x2C, 0xA2, 0xAC }, // Green
	{ 0xB8, 0xB8, 0x98, 0x88, 0x88, 0x86, 0xA8, 0xA6 }, // Red
};

//
const SpeedData g_SpeedData[SPEED_MAX] = 
{
	{  8, 800 }, // Normal 
	{  4, 850 }, // Turbo
	{ 16, 500 }, // Snail
	{ 12, 600 }, // Chill
};

//
const u16 g_ClearBG[] = { (u16)(((TILE_EMPTY + 1) << 8) + TILE_EMPTY), (u16)((TILE_EMPTY << 8) + (TILE_EMPTY + 1)) };

// Training levels list
const u8* g_TrainLevelList[] =
{ 
	g_Level001, g_Level002, g_Level003, g_Level004, g_Level005,
	g_Level006, g_Level007, g_Level008, g_Level009, g_Level010,
	g_Level011, g_Level012, g_Level013, g_Level014, g_Level015,
	g_Level016, g_Level017, g_Level018, g_Level019, g_Level020,
	g_Level021, g_Level022, g_Level023, g_Level024, g_Level025,
	g_Level026, g_Level027, g_Level028, g_Level029, g_Level030,
	g_Level031, g_Level032, g_Level033, g_Level034, g_Level035,
	g_Level036, g_Level037, g_Level038, g_Level039, g_Level040
};

//=============================================================================
// MEMORY DATA
//=============================================================================

// System
bool		g_VBlank = FALSE;		// Vertical-synchronization flag
u8			g_Frame = 0;			// Frame counter
u8			g_6thFrameCount = 0;	// Frame counter
u8			g_Freq;
u8			g_FreqDetected;
u8			g_FreqOpt = FREQ_AUTO;
bool		g_DoSynch;
u8			g_PalOpt;
u8			g_VersionVDP;
u16			g_Scroll;
bool		g_Initialized = FALSE;
u8			g_StartPage = 0xFF;
u8			g_Continue;

// Audio
bool		g_OptMusic = DEF_MUSIC;
u8			g_OptMusicIdx = 0;
bool		g_OptSFX = DEF_SFX;
u8			g_OptSFXIdx = 0;
u8			g_OptSFXNum;
u8			g_LastMusicId = 0xFF;

// Gameplay
u8			g_GameMode = MODE_TRAINNNG;
u8			g_GameCount;
Player		g_Players[PLAYER_MAX];	// Players information
Player		g_TrainPlayer;	// Players information
u8			g_PlayerMax;
Vector		g_BonusPos;				// Bonus information
u8			g_BonusTile;
u8			g_BonusOpt = 3;
c8			g_StrBuffer[32];		// String temporary buffer
u8			g_ScreenBuffer[32*24];
u8			g_CurrentPlayer;
u8			g_WallNum = 0;
u8			g_WallOpt = 0;
u8			g_BonusLen = BONUS_GROWTH;
u8			g_CollapseTimer;
u8			g_CollapsePhase;
u8			g_CollapseX0;
u8			g_CollapseY0;
u8			g_CollapseX1;
u8			g_CollapseY1;
Player*		g_Winner;				// Winner player index
u8			g_Speed = SPEED_NORMAL;
MenuItemMinMax g_MenuLevelMinMax;

// Solo mode
Vector		g_PlayerStart;
u8			g_BonusNum;
u8			g_TrainLevel = 0;		// Current training level [1~40]
u16			g_TrainScore[TRAIN_LEVEL_MAX];
u16			g_TrainTotal = 0;
u8			g_HiLevel = 0;
u16			g_HiScore[TRAIN_LEVEL_MAX];
u16			g_HiTotal = 0;

// Timers
u8			g_Counter;
u8			g_TimeFrame;
u8			g_TimeMinHigh;
u8			g_TimeMinLow;
u8			g_TimeSecHigh;
u8			g_TimeSecLow;
bool		g_HurryUp = FALSE;

// Input
u8			g_JoyInfo;
u8			g_JoyNum;
u8			g_Input[CTRL_PLY_NUM];
u8			g_SlotIdx;
bool		g_SelectEdit;
u8			g_CtrlReg[CTRL_MAX];
u8			g_MenuInputPrev = 0xFF;
u8			g_CtrlTurn = TURN_ABSOLUTE;
bool		g_Cheat = FALSE;

//=============================================================================
// FUNCTIONS
//=============================================================================

//.............................................................................
// INPUT
//.............................................................................

//-----------------------------------------------------------------------------
// Check if up drection is pressed
bool IsInputUp()
{
	if (Keyboard_IsKeyPushed(KEY_UP))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_UP))
			return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Check if down drection is pressed
bool IsInputDown()
{
	if (Keyboard_IsKeyPushed(KEY_DOWN))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_DOWN))
			return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Check if left drection is pressed
bool IsInputLeft()
{
	if (Keyboard_IsKeyPushed(KEY_LEFT))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_LEFT))
			return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Check if right drection is pressed
bool IsInputRight()
{
	if (Keyboard_IsKeyPushed(KEY_RIGHT))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_RIGHT))
			return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Check if trigger A is pressed
bool IsInputButton1()
{
	if (Keyboard_IsKeyPushed(KEY_SPACE))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_A))
			return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Check if trigger B is pressed
bool IsInputButton2()
{
	if (Keyboard_IsKeyPushed(KEY_ESC))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPushed(i, NTAP_B))
			return TRUE;

	return FALSE;
}

//.............................................................................
// AUDIO
//.............................................................................

//-----------------------------------------------------------------------------
// Play the given music
void PlayMusic(u8 id)
{
	if (!g_OptMusic)
		id = MUSIC_EMPTY;

	// Load Music data
	const void* mus = g_MusicInfo[id];
	if (g_LastMusicId != id)
	{
		AKG_Stop();
		g_LastMusicId = id;
		Pletter_UnpackToRAM(mus, (void*)0xD000);
	}
	AKG_Init((const void*)0xD000, 0);

	// Load SFX data
	Pletter_UnpackToRAM(g_DataSFX, (void*)0xE800);
	g_OptSFXNum = AKG_InitSFX((const void*)0xE800);
}

//-----------------------------------------------------------------------------
// Play the given sound effect
void PlaySFX(u8 id)
{
	if (!g_OptSFX)
		return;

	AKG_PlaySFX(id, ARKOS_CHANNEL_C, 0);
}

//.............................................................................
// ...
//.............................................................................

//-----------------------------------------------------------------------------
// Load patterns in all 3 screen sections
void VDP_LoadPattern_GM2_Pletter(const u8* src, u8 offset)
{
	u16 dst = g_ScreenPatternLow + (offset * 8);
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
}

//-----------------------------------------------------------------------------
// Load colors in all 3 screen sections
void VDP_LoadColor_GM2_Pletter(const u8* src, u8 offset)
{
	u16 dst = g_ScreenColorLow + (offset * 8);
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
}

//-----------------------------------------------------------------------------
// 
inline void PrintChr(u8 x, u8 y, c8 chr)
{
	VDP_Poke_GM2(x++, y, chr);
}

//-----------------------------------------------------------------------------
// 
inline void PrintChrX(u8 x, u8 y, c8 chr, u8 len)
{
	VDP_FillLayout_GM2(chr, x, y, len, 1);
}

//-----------------------------------------------------------------------------
// 
void PrintChrY(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		VDP_Poke_GM2(x, y++, chr);
}

//-----------------------------------------------------------------------------
// Clear the frame buffer in RAM
inline void ClearLevel()
{
	u8* ptr = g_ScreenBuffer;
	for(u8 i = 0; i < 24; ++i)
	{
		Mem_Set_16b(g_ClearBG[i & 1], ptr, 32);
		ptr += 32;
	}
}

//-----------------------------------------------------------------------------
// Draw the frame buffer in RAM to the VRAM at once
inline void DrawLevel()
{
	VDP_WriteVRAM(g_ScreenBuffer, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32 * 24);
}

//-----------------------------------------------------------------------------
// Get tile value from the RAM frame buffer
inline u8 GetTile(u8 x, u8 y)
{
	return g_ScreenBuffer[x + (y * 32)];
}

//-----------------------------------------------------------------------------
// Draw a tile in he frame buffer in RAM
inline void DrawTile(u8 x, u8 y, c8 chr)
{
	g_ScreenBuffer[x + (y * 32)] = chr;
}

//-----------------------------------------------------------------------------
// Draw a horizontal line of tiles
void DrawTileX(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		DrawTile(x++, y, chr);
}

//-----------------------------------------------------------------------------
// Draw a verical line of tiles
void DrawTileY(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		DrawTile(x, y++, chr);
}

//-----------------------------------------------------------------------------
//
u8 GetWallTile()
{
	u8 tile = g_WallData[g_WallOpt];
	if (tile == 0)
		tile = g_WallData[Math_GetRandom8() & 0x03];
	return tile;
}

//-----------------------------------------------------------------------------
// Uncompress the give training field data
void UnpackTrainField(u8 id)
{
	u8* ptr = (u8*)Mem_GetHeapAddress();
	Pletter_UnpackToRAM(g_TrainLevelList[id], ptr);

	g_BonusNum = 0;

	u8 x = 1;
	u8 y = 2;
	for(u8 i = 0; i < 32 * 21 / 4; ++i)
	{
		u8 val = *ptr;
		for(u8 j = 0; j < 4; ++j)
		{
			u8 tile = 0;
			switch(val & 0x3)
			{
			case 1:
				tile = GetWallTile();
				break;
			case 2:
				tile = TILE_HEART;
				g_BonusNum++;
				break;
			case 3:
				tile = TILE_PREHOLE;
				g_PlayerStart.X = x;
				g_PlayerStart.Y = y;
				break;
			}
			if (tile)
				DrawTile(x, y, tile);
			x++;
			val >>= 2;
		}

		if (x > 30)
		{
			x = 1;
			y++;
		}

		ptr++;
	}
}

//-----------------------------------------------------------------------------
// Update the player score on screen
inline void SetScore(Player* ply)
{
	Print_DrawIntAt(ply->ID * 4 + 1, 0, ply->Score);
}

//-----------------------------------------------------------------------------
// Draw the game start countdown
void DrawCounter(u8 x, u8 y, u8 step)
{
	u8 x0 = x;
	u16 bit = 0b0100000000000000;
	for(u8 i = 15; i > 0; --i)
	{
		if (g_TimerLayout[step] & bit)
			PrintChr(x, y, TILE_BALL);
		else
			PrintChr(x, y, TILE_EMPTY);
		x++;
		if (x > x0 + 2)
		{
			x -= 3;
			y++;
		}
		bit >>= 1;
	}
	// u8 col = step > 4 ? 2 : step & 0x1;
	// VDP_LoadColor_GM2(g_BallColor[col], 1, TILE_BALL);
}

//-----------------------------------------------------------------------------
// Set the gameplay timer value (in minutes)
void SetTimer(u8 min)
{
	g_TimeFrame = 0;
	g_TimeMinHigh = Math_Div10(min);
	g_TimeMinLow = Math_Mod10(min);
	g_TimeSecHigh = 0;
	g_TimeSecLow = 0;
	g_CollapseTimer = 0;
	g_CollapsePhase = COLLAPSE_OFF;
	if (g_GameMode != MODE_TRAINNNG && g_HurryUp)
	{
		PlayMusic(MUSIC_BATTLE);
		g_HurryUp = FALSE;
	}

	VDP_SetSpriteSM1(4, 117, 185, 16 + g_TimeMinHigh, COLOR_WHITE);
	VDP_SetSpriteSM1(5, 122, 185, 16 + g_TimeMinLow,  COLOR_WHITE);
	VDP_SetSpriteSM1(6, 131, 185, 16 + g_TimeSecHigh, COLOR_WHITE);
	VDP_SetSpriteSM1(7, 136, 185, 16 + g_TimeSecLow,  COLOR_WHITE);
}

//-----------------------------------------------------------------------------
// Dicrease the gameplayer timer
bool UpdateTimer()
{
	// Blink the timer when less than 30 seconds remains
	if (((g_TimeMinHigh | g_TimeMinLow) == 0) && (g_TimeSecHigh < 3))
	{
		u8 col = ((g_Frame >> 4) & 0x1) ? COLOR_WHITE : COLOR_LIGHT_RED;
		VDP_SetSpriteColorSM1(4, col);
		VDP_SetSpriteColorSM1(5, col);
		VDP_SetSpriteColorSM1(6, col);
		VDP_SetSpriteColorSM1(7, col);
		
		if (!g_HurryUp)
		{
			PlayMusic(MUSIC_HURRYUP);
			g_HurryUp = TRUE;
		}
	}

	g_TimeFrame--;
	if (g_TimeFrame == 255)
	{
		g_TimeFrame = 50;
		g_TimeSecLow--;
		if (g_TimeSecLow == 255)
		{
			g_TimeSecLow = 9;
			g_TimeSecHigh--;
			if (g_TimeSecHigh == 255)
			{
				g_TimeSecHigh = 5;
				g_TimeMinLow--;
				if (g_TimeMinLow == 255)
				{
					g_TimeMinLow = 9;
					g_TimeMinHigh--;
					if (g_TimeMinHigh == 255)
						g_TimeMinHigh = 5;
				}
			}
		}

		VDP_SetSpritePattern(4, 16 + g_TimeMinHigh);
		VDP_SetSpritePattern(5, 16 + g_TimeMinLow);
		VDP_SetSpritePattern(6, 16 + g_TimeSecHigh);
		VDP_SetSpritePattern(7, 16 + g_TimeSecLow);
	}

	return g_TimeMinHigh | g_TimeMinLow | g_TimeSecHigh | g_TimeSecLow;
}

//-----------------------------------------------------------------------------
// Increment the gameplayer timer
bool IncTimer()
{
	g_TimeFrame++;
	if (g_TimeFrame == 50)
	{
		g_TimeFrame = 0;
		g_TimeSecLow++;
		if (g_TimeSecLow == 10)
		{
			g_TimeSecLow = 0;
			g_TimeSecHigh++;
			if (g_TimeSecHigh == 6)
			{
				g_TimeSecHigh = 0;
				g_TimeMinLow++;
				if (g_TimeMinLow == 10)
				{
					g_TimeMinLow = 0;
					g_TimeMinHigh++;
					if (g_TimeMinHigh == 6)
						g_TimeMinHigh = 0;
				}
			}
		}

		VDP_SetSpritePattern(4, 16 + g_TimeMinHigh);
		VDP_SetSpritePattern(5, 16 + g_TimeMinLow);
		VDP_SetSpritePattern(6, 16 + g_TimeSecHigh);
		VDP_SetSpritePattern(7, 16 + g_TimeSecLow);
	}
}

//-----------------------------------------------------------------------------
// Move selection cursor to the given button map index 
void MoveCursor(i8 idx)
{
	idx %= numberof(g_BattleSelectSlot);
	g_SlotIdx = idx;

	VDP_SetSpriteSM1(0, 0, 0, 8, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(1, 0, 0, 9, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(2, 0, 0, 10, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(3, 0, 0, 11, COLOR_DARK_BLUE);
}

//-----------------------------------------------------------------------------
// Display the player edit interface
void EditPlayer(u8 id, bool bEdit)
{
	// Display controller selection
	u8 x = g_CharaInfo[id].FrameX;
	u8 y = g_CharaInfo[id].FrameY;
	u8 ctrl = g_Players[id].Controller;
	VDP_WriteLayout_GM2(bEdit ? g_DeviceSelect[ctrl].Edit : g_DeviceSelect[ctrl].Default, x, y + 6, 7, 3);
	
	// Display turn mode selection
	u8 yOffset = 0;
	if (g_CtrlTurn == TURN_CUSTOM)
	{
		u8 turn = g_Players[id].Turn;
		VDP_WriteLayout_GM2(bEdit ? g_TurnSelect[turn].Edit : g_TurnSelect[turn].Default, x, y - 1, 7, 2);
		yOffset = 12;
	}
	
	// Set selection sprite position
	if (bEdit)
	{
		const SelectSlot* slot = &g_BattleSelectSlot[g_SlotIdx];
		VDP_SetSpritePosition(0, slot->UL.X - 1, slot->UL.Y - yOffset);
		VDP_SetSpritePosition(1, slot->DR.X + 1, slot->UL.Y - yOffset);
		VDP_SetSpritePosition(2, slot->UL.X - 1, slot->DR.Y + 17);
		VDP_SetSpritePosition(3, slot->DR.X + 1, slot->DR.Y + 17);
	}

	g_SelectEdit = bEdit;
}

//-----------------------------------------------------------------------------
// Get remaining alive human players count
u8 GetHumanCount()
{
	u8 count = 0;
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		if ((g_Players[i].Controller < CTRL_PLY_NUM) && (g_Players[i].State != STATE_NONE))
			count++;
	return count;
}

//-----------------------------------------------------------------------------
// Check Battle Royal game mode condition. Called at each player death
void CheckBattleRoyal()
{
	g_DoSynch = (GetHumanCount() > 0);

	// Check is there is a last standing
	Player* lastPly = NULL;
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		if ((g_Players[i].Controller != CTRL_NONE) && (g_Players[i].State != STATE_NONE))
		{
			if (lastPly)
				return;
			lastPly = &g_Players[i];
		}
	}

	// Score a point
	lastPly->Score++;
	SetScore(lastPly);

	// Check victory condition
	if (lastPly->Score >= g_GameCount)
	{
		g_Winner = lastPly;
		FSM_SetState(&State_Victory);
		return;
	}

	// Clean field and start a new round
	SetTimer(BATTLEROYAL_TIME);
	DrawLevel();
	ClearPlayer(lastPly);
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		SpawnPlayer(&g_Players[i]);
	g_DoSynch = (GetHumanCount() > 0);
}

//-----------------------------------------------------------------------------
// 
bool CheckDeathMatch(Player* ply, u8 cell)
{
	// Check opponant
	cell -= 64;
	u8 id = 0xFF;
	for(u8 i = 0; i < 8; ++i)
	{
		if (cell < 20)
		{
			id = i;
			break;
		}
		cell -= 20;
	}

	// Loss a point for self or background collision
	if ((id == 0xFF) || (id == ply->ID))
	{
		if (ply->Score > 0)
		{
			ply->Score--;
			SetScore(ply);
		}
	}
	else
	{
		Player* op = &g_Players[id];
		op->Score++;
		SetScore(op);

		// Check victory condition
		if (op->Score >= g_GameCount)
		{
			g_Winner = op;
			FSM_SetState(&State_Victory);
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
//
void CheckSizeMatter()
{
	// Check the longuest crawlers
	Player* maxPly = &g_Players[0];
	for(u8 i = 1; i < PLAYER_MAX; ++i)
		if (g_Players[i].Controller != CTRL_NONE)
			if (g_Players[i].Score > maxPly->Score)
				maxPly = &g_Players[i];

	g_Winner = maxPly;
	FSM_SetState(&State_Victory);
}

//-----------------------------------------------------------------------------
//
bool CheckGreediest(Player* ply)
{
	if (ply->Score >= g_GameCount) // check victory count
	{
		g_Winner = ply;
		FSM_SetState(&State_Victory);
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
//
u16 GetTotalTrainingScore(const u16* tab, u8 level)
{
	u16 score = 0;
	loop(i, level + 1)
		score += tab[i];
	return score;
}

//-----------------------------------------------------------------------------
// Find a free spot to spawn a bonus
void SpawnBonus()
{
	if (g_CollapsePhase != COLLAPSE_OFF)
		return;

	// Search a free spot
	u8 x = 0;
	u8 y = 0;
	do
	{
		u8 rnd = Math_GetRandom8();
		x = 8 + rnd % 16;
		y = 8 + (rnd >> 4) % 8;
	}
	while(VDP_Peek_GM2(x, y) != TILE_EMPTY);

	// Spawn the bonus
	g_BonusPos.X = x;
	g_BonusPos.Y = y;
	g_BonusTile = g_BonusData[g_BonusOpt];
	if (g_BonusTile == 0) // Random bonus tile
	{
		u8 rnd = Math_GetRandom8() % 8;
		g_BonusTile = g_BonusData[rnd];
	}
	VDP_Poke_GM2(x, y, g_BonusTile);
}

//-----------------------------------------------------------------------------
//
u8 CheckDir(u8 x, u8 y, u8 dir, u8 max)
{
	u8 dist = 0;
	while(dist < max)
	{
		switch(dir)
		{
		case DIR_UP:	y--; break;
		case DIR_RIGHT:	x++; break;
		case DIR_DOWN:	y++; break;
		case DIR_LEFT:	x--; break;
		}
		if (VDP_Peek_GM2(x, y) < 0xF0)
			return dist;
		dist++;
	}
	return max;
}

const i8 g_DistWeight[] = { 10, 8, 4, 1 };

//-----------------------------------------------------------------------------
//
void UpdateAI(Player* ply)
{
	u8 rnd = Math_GetRandom8();

	if ((ply->Controller == CTRL_AI_EASY) && (rnd & 0x03 != 0)) // 3 in 4 chances to do nothing
		return;
	else if ((ply->Controller == CTRL_AI_MED) && (rnd & 0x03 == 0)) // 1 in 4 chances to do nothing
		return;

	i8 weight[ACTION_MAX] = { 0, 0, 0 };

	u8 x = ply->PosX;
	u8 y = ply->PosY;

	// Front
	u8 dist = CheckDir(x, y, ply->Dir, 3);
	if (dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_LEFT] += w;
		weight[ACTION_RIGHT] += w;
	}

	// Check right
	dist = CheckDir(x, y, (ply->Dir + 1) & 0x3 /*% DIR_MAX*/, 3);
	if (dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_NONE] += w;
		weight[ACTION_LEFT] += w;
	}

	// Check left
	dist = CheckDir(x, y, (ply->Dir + DIR_MAX - 1) & 0x3 /*% DIR_MAX*/, 3);
	if (dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_NONE] += w;
		weight[ACTION_RIGHT] += w;
	}

	// Seek Bonus
	switch(ply->Dir)
	{
	case DIR_UP:
		if (g_BonusPos.X > ply->PosX)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if (g_BonusPos.X < ply->PosX)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_DOWN:
		if (g_BonusPos.X < ply->PosX)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if (g_BonusPos.X > ply->PosX)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_RIGHT:
		if (g_BonusPos.Y > ply->PosY)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if (g_BonusPos.Y < ply->PosY)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_LEFT:
		if (g_BonusPos.Y < ply->PosY)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if (g_BonusPos.Y > ply->PosY)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;
	}

	// Randomize
	weight[ACTION_NONE] += rnd & 0x07;

	// Select the choice with the biggest weight
	u8 choice = ACTION_NONE;
	if (weight[ACTION_NONE] < weight[ACTION_RIGHT])
		choice = ACTION_RIGHT;
	else if ((weight[ACTION_NONE] == weight[ACTION_RIGHT]) && (rnd & 0x01))
		choice = ACTION_RIGHT;
	if (weight[choice] < weight[ACTION_LEFT])
		choice = ACTION_LEFT;
	else if ((weight[choice] == weight[ACTION_LEFT]) && (rnd & 0x01))
		choice = ACTION_LEFT;

	// Apply choice
	if (choice == ACTION_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if (choice == ACTION_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
}

//-----------------------------------------------------------------------------
//
void UpdatePlayerInput(Player* ply)
{
	u8 act = ACTION_NONE;

	if (ply->Turn == TURN_RELATIVE)
	{
		if (g_Input[ply->Controller] == DIR_RIGHT)
			act = ACTION_RIGHT;
		else if (g_Input[ply->Controller] == DIR_LEFT)
			act = ACTION_LEFT;
	}
	else // if (ply->Turn == TURN_ABSOLUTE)
	{
		if (g_Input[ply->Controller] == (ply->Dir + 1) % DIR_MAX)
			act = ACTION_RIGHT;
		else if (g_Input[ply->Controller] == (ply->Dir + DIR_MAX - 1) % DIR_MAX)
			act = ACTION_LEFT;
	}

	if (act == ACTION_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if (act == ACTION_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
	g_Input[ply->Controller] = DIR_MAX;
}

//-----------------------------------------------------------------------------
// 
void SetPlayerController(Player* ply, u8 ctrl)
{
	// Register device
	if (ply->Controller < CTRL_PLY_NUM) // Free previous
		g_CtrlReg[ply->Controller] = CTRL_FREE;
	if (ctrl < CTRL_PLY_NUM) // Reserve next
		g_CtrlReg[ctrl] = ply->ID;

	ply->Controller  = ctrl;

	switch(ctrl)
	{
	case CTRL_JOY_1:
	case CTRL_JOY_2:
	case CTRL_JOY_3:
	case CTRL_JOY_4:
	case CTRL_JOY_5:
	case CTRL_JOY_6:
	case CTRL_JOY_7:
	case CTRL_JOY_8:
	case CTRL_KEY_1:
	case CTRL_KEY_2:
		ply->Action = UpdatePlayerInput;
		break;
	case CTRL_AI_EASY:
	case CTRL_AI_MED:
	case CTRL_AI_HARD:
		ply->Action = UpdateAI;
		break;
	case CTRL_NONE:
		ply->Action = NULL;
		break;
	};
}

//-----------------------------------------------------------------------------
// 
void SetPrevPlayerController(Player* ply)
{
	u8 ctrl = ply->Controller;
	bool bLoop = TRUE;
	while(bLoop)
	{
		if (ctrl > 0)
			ctrl--;
		else
			ctrl = CTRL_MAX - 1;
		if ((ctrl <= CTRL_JOY_8) && (ctrl >= g_JoyNum))
			continue;
		if (g_CtrlReg[ctrl] == CTRL_FREE)
			break;
	}
	SetPlayerController(ply, ctrl);
}

//-----------------------------------------------------------------------------
// 
void SetNextPlayerController(Player* ply)
{
	u8 ctrl = ply->Controller;
	bool bLoop = TRUE;
	while(bLoop)
	{
		if (ctrl < CTRL_MAX - 1)
			ctrl++;
		else
			ctrl = 0;
		if ((ctrl <= CTRL_JOY_8) && (ctrl >= g_JoyNum))
			continue;
		if (g_CtrlReg[ctrl] == CTRL_FREE)
			break;
	}
	SetPlayerController(ply, ctrl);
}

//-----------------------------------------------------------------------------
// 
void InitPlayer(Player* ply, u8 id)
{
	ply->ID         = id;
	ply->Score      = 0;
	ply->Controller = CTRL_FREE;
	ply->Turn       = (g_CtrlTurn == TURN_ABSOLUTE) ? TURN_ABSOLUTE : TURN_RELATIVE;
	ply->State      = STATE_NONE;

	if (id < g_JoyNum)
		SetPlayerController(ply, id);
	else if (id < g_PlayerMax)
		SetPlayerController(ply, CTRL_KEY_1 + id - g_JoyNum);
	else
		SetPlayerController(ply, CTRL_AI_MED);
}

//-----------------------------------------------------------------------------
// 
void ResetPlayer(Player* ply)
{
	ply->Score      = 0;

	SpawnPlayer(ply);
}

//-----------------------------------------------------------------------------
// 
void SpawnPlayer(Player* ply)
{
	if (ply->Controller == CTRL_NONE)
		return;
	
	const Start* start = &g_Starts[ply->ID];
	ply->PosX   = start->X;
	ply->PosY   = start->Y;
	ply->Dir    = start->Dir;
	ply->Length = LENGTH_MIN;
	ply->Expect = LENGTH_DEFAULT;
	ply->Anim   = 0;
	ply->Idx    = 0;
	ply->State  = STATE_INIT;
	ply->Timer  = 0;
	for(u8 i = 0; i < LENGTH_MIN; ++i)
		ply->Path[i] = start->Dir;

	if(g_GameMode != MODE_TRAINNNG)
		SetScore(ply);
}

//-----------------------------------------------------------------------------
//
void DrawPlayer(Player* ply, u8 x, u8 y)
{
	ply->PosX = x;
	ply->PosY = y;
	ply->Idx--;
	ply->Idx %= LENGTH_MAX;
	ply->Path[ply->Idx] = ply->Dir;

	u8 idx = ply->Idx;
	u8 baseTile = g_CharaInfo[ply->ID].TileBase;
	bool bGrow = FALSE;
	bool bReduce = FALSE;

	// Grow crawler length
	if ((ply->Length < ply->Expect) && (ply->Length < LENGTH_MAX))
	{
		ply->Length++;
		if ((g_GameMode == MODE_SIZEMATTER) && (ply->Length > ply->Score))
		{
			ply->Score = ply->Length;
			SetScore(ply);
		}
		bGrow = TRUE;
	}
	// Reduce crawler length
	else if ((ply->Length > ply->Expect) && (ply->Length > LENGTH_MIN))
	{
		ply->Length--;
		bReduce = TRUE;
	}

	// Draw crawler
	for(u8 i = 0; i < ply->Length + 1; ++i)
	{
		// Head
		if (i == 0)
		{
			u8 tile = 0x40 + ply->Path[idx];
			VDP_Poke_GM2(x, y, tile + baseTile);

			// Sprite
			const Character* chr = &g_CharaInfo[ply->ID];
			if (chr->Sprite != 0xFF)
			{
				if (chr->Sprite & 1) // Snake
				{
					u8 sx = x;
					u8 sy = y;
					switch(ply->Path[idx])
					{
					case DIR_UP:	sy--; break;
					case DIR_RIGHT:	sx++; break;
					case DIR_DOWN:	sy++; break;
					case DIR_LEFT:	sx--; break;
					}
					VDP_SetSpriteSM1(chr->Sprite, sx * 8, sy * 8 - 1, 4 + ply->Path[idx], COLOR_DARK_RED);
				}
				else // Caterpillar
				{
					VDP_SetSpriteSM1(chr->Sprite, x * 8, (y - 1 )* 8 - 1, ply->Path[idx], COLOR_BLACK);
				}
			}
		}
		// Body
		else if ((i == 1) && (ply->Length > 2))
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			const Shapes* vec = &g_Body[ply->Path[prev] + (ply->Path[idx] << 2)];
			u8 tile;
			if (idx & 1)
				tile = vec->A;
			else
				tile = vec->B;
			if (VDP_Peek_GM2(x, y) != TILE_HOLE)
				VDP_Poke_GM2(x, y, tile + baseTile);
		}
		// Tail
		else if (i == ply->Length - 1)
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			u8 tile = 0x4C + ply->Path[prev] * 2;
			if (ply->Anim & 1)
				tile++;
			if (VDP_Peek_GM2(x, y) != TILE_HOLE)
				VDP_Poke_GM2(x, y, tile + baseTile);
		}
		// Clear
		else if ((!bGrow) && (i == ply->Length))
		{
			if (VDP_Peek_GM2(x, y) != TILE_HOLE)
				VDP_Poke_GM2(x, y, g_ScreenBuffer[x + (y * 32)]);
		}

		switch(ply->Path[idx])
		{
		case DIR_UP:	y++; break;
		case DIR_RIGHT:	x--; break;
		case DIR_DOWN:	y--; break;
		case DIR_LEFT:	x++; break;
		}
		idx++;
		idx %= LENGTH_MAX;
	}
	ply->Anim++;
}

//-----------------------------------------------------------------------------
//
void ClearPlayer(Player* ply)
{
	PlaySFX(SFX_DEATH);

	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;

	// Clear tiles
	for(u8 i = 0; i < ply->Length; ++i)
	{
		if (VDP_Peek_GM2(x, y) != TILE_HOLE)
			VDP_Poke_GM2(x, y, g_ScreenBuffer[x + (y * 32)]);

		switch(ply->Path[idx])
		{
		case DIR_UP:	y++; break;
		case DIR_RIGHT:	x--; break;
		case DIR_DOWN:	y--; break;
		case DIR_LEFT:	x++; break;
		}
		idx++;
		idx %= LENGTH_MAX;
	}

	// Clear spite
	const Character* chr = &g_CharaInfo[ply->ID];
	if (chr->Sprite != 0xFF)
		VDP_HideSprite(chr->Sprite);

	ply->State = STATE_NONE;
}

//-----------------------------------------------------------------------------
// Program entry point
void UpdatePlayer(Player* ply)
{
	if (ply->Controller == CTRL_NONE)
		return;

	switch(ply->State)
	{
	case STATE_NONE:
		return;

	case STATE_INIT:
		PlaySFX(SFX_HOLE);
		ply->Timer = COOLDOWN_WAIT;
		ply->State = STATE_COOLDOWN;

	case STATE_COOLDOWN:
		if (VDP_Peek_GM2(ply->PosX, ply->PosY) >= TILE_EMPTY)
		{
			u8 tile = (u8)(TILE_PREHOLE + 1);
			if (ply->Timer <= (COOLDOWN_WAIT / 4))
				tile++;
			VDP_Poke_GM2(ply->PosX, ply->PosY, tile);
		}
		ply->Timer--;
		if (ply->Timer)
			return;
		ply->State = STATE_WAITING;

	case STATE_WAITING:
		if (VDP_Peek_GM2(ply->PosX, ply->PosY) < TILE_EMPTY)
			return;
		ply->Timer = SPAWN_WAIT;
		ply->State = STATE_SPAWNING;

	case STATE_SPAWNING:
	{
		// Get input action
		ply->Action(ply);

		// Diplay hole
		u8 tile = TILE_HOLE;
		if ((ply->Timer < (3 * SPAWN_WAIT / 4)) && (ply->Timer & 1))
			tile += 1 + ply->Dir;
		VDP_Poke_GM2(ply->PosX, ply->PosY, tile);

		// Check timer
		ply->Timer--;
		if (ply->Timer)
			return;
		ply->State = STATE_PLAYING;
	}

	case STATE_PLAYING:
	{
		// Get input action
		ply->Action(ply);

		// Move
		u8 x = ply->PosX;
		u8 y = ply->PosY;
		switch(ply->Dir)
		{
		case DIR_UP:	y--; break;
		case DIR_RIGHT:	x++; break;
		case DIR_DOWN:	y++; break;
		case DIR_LEFT:	x--; break;
		}

		// Check destination
		u8 cell = VDP_Peek_GM2(x, y);
		if (cell < 0xF0) // Obstacle
		{
			ClearPlayer(ply);
			switch(g_GameMode)
			{
			case MODE_BATTLEROYAL:
				CheckBattleRoyal();
				return;
			case MODE_DEATHMATCH:
				if (!CheckDeathMatch(ply, cell))
					return;
			case MODE_SIZEMATTER:
			case MODE_GREEDIEST:
				SpawnPlayer(ply);
				return;
			case MODE_TRAINNNG:
				State_TrainGame_Begin(); // Restart level from sratch
				return;
			};
		}
		else // No obstacle
		{
			switch(cell)
			{
			case TILE_BONUS:
			case TILE_BONUS+1:
			case TILE_BONUS+2:
			case TILE_BONUS+3:
			case TILE_BONUS+4:
			case TILE_BONUS+5:
			case TILE_BONUS+6:
			case TILE_HEART:
				PlaySFX(SFX_BONUS);
				ply->Expect += g_BonusLen;
				if (g_GameMode == MODE_TRAINNNG)
				{
					DrawTile(x, y, 0xF2);
					g_BonusNum--;
					if (g_BonusNum == 0) // Got the last bonus!
					{
						PlaySFX(SFX_VICTORY);
						FSM_SetState(&State_TrainScore);
						return;
					}
				}
				else
					SpawnBonus();
				if (g_GameMode == MODE_GREEDIEST)
				{
					ply->Score++;
					SetScore(ply);
					CheckGreediest(ply); // Check victory condition
				}
			default:
				DrawPlayer(ply, x, y);
				break;
			}
		}
		return;
	}
	}
}

void Menu_DisplayItem(u8);

//-----------------------------------------------------------------------------
// 
void SetGameMode(u8 newMode)
{
	g_GameMode = newMode;

	const ModeInfo* info = &g_ModeInfo[g_GameMode];
	g_GameCount = info->Count;
	g_BonusLen  = info->Bonus;

	// Update count number
	Menu_DisplayItem(2);
}

//-----------------------------------------------------------------------------
//
const c8* ScrollString(const c8* src, u16 size)
{
	if (g_Scroll == size)
		g_Scroll = 0;

	u16 j = g_Scroll;
	for(u8 i = 0; i < STR_SCROLL_LEN; ++i)
	{
		g_StrBuffer[i] = src[j];
		if (++j >= size)
			j -= size;
	}
	g_StrBuffer[STR_SCROLL_LEN] = 0;

	g_Scroll++;

	return g_StrBuffer;
}


//-----------------------------------------------------------------------------
// VBlank interrupt
void VDP_InterruptHandler()
{
	g_VBlank = TRUE;

	g_6thFrameCount++;
	if (g_6thFrameCount == 5)
		g_6thFrameCount = 0;

	if ((g_Freq == FREQ_50HZ) || (g_6thFrameCount))
	{
		AKG_Decode();
	}
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
wait_loop:
	while(!g_VBlank) {} // Wait for VDP interruption
	g_VBlank = FALSE;

	// Skip the 6th frame on 60 Hz VDP
	if ((g_Freq == FREQ_60HZ) && (!g_6thFrameCount))
		goto wait_loop; // Wait 1 more frame

	g_Frame++;
}

//=============================================================================
// MENU
//=============================================================================

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Start(u8 op, i8 value)
{
	value;
	if (op == MENU_ACTION_SET)
	{
		switch(value)
		{
			case START_BATTLE:
				FSM_SetState(&State_BattleSelect);
				break;
			case START_TRAIN_NEW:
				g_Continue = FALSE;
				FSM_SetState(&State_TrainSelect);
				break;
			case START_TRAIN_CONTINUE:
				g_Continue = TRUE;
				FSM_SetState(&State_TrainSelect);
				break;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Mode(u8 op, i8 value)
{
	value;
	
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		if (g_GameMode < MODE_BATTLE_MAX - 1)
			SetGameMode(g_GameMode + 1);
		else
			SetGameMode(0);
		g_Scroll = 0;
		break;

	case MENU_ACTION_DEC:
		if (g_GameMode > 0)
			SetGameMode(g_GameMode - 1);
		else
			SetGameMode(MODE_BATTLE_MAX - 1);
		g_Scroll = 0;
		break;

	case MENU_ACTION_GET:
		return g_ModeInfo[g_GameMode].Name;
	};
	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Speed(u8 op, i8 value)
{
	value;
	
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_Speed = (g_Speed + 1) % SPEED_MAX;
		break;

	case MENU_ACTION_DEC:
		g_Speed = (g_Speed + SPEED_MAX - 1) % SPEED_MAX;
		break;

	case MENU_ACTION_GET:
		switch(g_Speed)
		{
		case SPEED_NORMAL: return "NORMAL \x1F\x1F\x1F"; 
		case SPEED_TURBO:  return "TURBO  \x1F\x1F\x1F\x1F"; 
		case SPEED_SNAIL:  return "SNAIL  \x1F"; 
		case SPEED_CHILL:  return "CHILL  \x1F\x1F"; 
		}
	};
	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Info(u8 op, i8 value)
{
	op;
	value;
	if (g_Frame & 0x7)
		return NULL;

	if (g_MenuItem != 1)
		return "";

	if (g_Frame & 0x7)
		return NULL;

	const ModeInfo* info = &g_ModeInfo[g_GameMode];
	return ScrollString(info->Desc, info->Length - 1);
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Credits(u8 op, i8 value)
{
	op;
	value;
	if (g_Frame & 0x7)
		return NULL;

	return ScrollString(g_TextCredits, sizeof(g_TextCredits) - 1);
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Freq(u8 op, i8 value)
{
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_FreqOpt = (g_FreqOpt + 1) % FREQ_MAX;
		break;
	case MENU_ACTION_DEC:
		g_FreqOpt = (g_FreqOpt + (FREQ_MAX - 1)) % FREQ_MAX;
		break;
	}

	if (g_FreqOpt == FREQ_60HZ) 
	{
		g_Freq = FREQ_60HZ;
		return "60 HZ";
	}
	else if (g_FreqOpt == FREQ_50HZ)
	{
		g_Freq = FREQ_50HZ;
		return "50 HZ";
	}
	else
	{
		g_Freq = g_FreqDetected;
		if (g_Freq == FREQ_50HZ)
			return "AUTO (50 HZ)";
		else
			return "AUTO (60 HZ)";
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
void ApplyTurnSetup()
{
	u8 turn = (g_CtrlTurn == TURN_ABSOLUTE) ? TURN_ABSOLUTE : TURN_RELATIVE;
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		g_Players[i].Turn = turn;

	g_TrainPlayer.Turn = turn;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Turn(u8 op, i8 value)
{
	value;
	switch (op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_CtrlTurn = (g_CtrlTurn + 1) % TURN_MAX;
		ApplyTurnSetup();
		break;
	case MENU_ACTION_DEC:
		g_CtrlTurn = (g_CtrlTurn + (TURN_MAX - 1)) % TURN_MAX;
		ApplyTurnSetup();
		break;
	}

	switch (g_CtrlTurn)
	{
	case TURN_RELATIVE:
		return "RELATIVE";
	case TURN_ABSOLUTE:
		return "ABSOLUTE";
	case TURN_CUSTOM:
		return "CUSTOM";
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Palette(u8 op, i8 value)
{
	value;
	if (g_VersionVDP == VDP_VERSION_TMS9918A)
		return "(FOR MSX2)";

	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_PalOpt = (g_PalOpt + 1) % PAL_MAX;
		break;
	case MENU_ACTION_DEC:
		g_PalOpt = (g_PalOpt + (PAL_MAX - 1)) % PAL_MAX;
		break;
	}

	switch(g_PalOpt)
	{
	case PAL_CUSTOM:
		VDP_SetPalette((u8*)g_MSX2Palette);
		return "CUSTOM";
	case PAL_MSX1: 
		VDP_SetMSX1Palette();
		return "MSX1";
	case PAL_MSX2: 
		VDP_SetDefaultPalette();
		return "MSX2";
	case PAL_GRAY: 
		VDP_SetPalette((u8*)g_GrayPalette);
		return "GRAY";
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Music(u8 op, i8 value)
{
	if(value == 0)
	{
		switch(op)
		{
		case MENU_ACTION_SET:
		case MENU_ACTION_INC:
		case MENU_ACTION_DEC:
			TOGGLE(g_OptMusic);
			PlayMusic(g_OptMusicIdx);
			break;
		}
		return g_OptMusic ? "*" : "/";
	}
	else
	{
		switch(op)
		{
		case MENU_ACTION_INC:
			if (g_OptMusicIdx < numberof(g_MusicInfo) - 2)
				g_OptMusicIdx++;
			else
				g_OptMusicIdx = 0;
			PlayMusic(g_OptMusicIdx);
			break;
		case MENU_ACTION_DEC:
			if (g_OptMusicIdx > 0)
				g_OptMusicIdx--;
			else
				g_OptMusicIdx =  numberof(g_MusicInfo) - 2;
			PlayMusic(g_OptMusicIdx);
			break;
		}

		switch(g_OptMusicIdx)
		{
			case MUSIC_MENU:    return "MENU";
			case MUSIC_BATTLE:  return "BATTLE";
			case MUSIC_HURRYUP: return "HURRYUP";
			case MUSIC_VICTORY: return "VICTORY";
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_SFX(u8 op, i8 value)
{
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
	case MENU_ACTION_DEC:
		TOGGLE(g_OptSFX);
		break;
	}
	return g_OptSFX ? "*" : "/";
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Bonus(u8 op, i8 value)
{
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_BonusOpt++;
		if (g_BonusOpt >= sizeof(g_BonusData))
			g_BonusOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_BonusOpt--;
		if (g_BonusOpt == 255)
			g_BonusOpt = sizeof(g_BonusData) - 1;
		break;
	}

	if (g_BonusData[g_BonusOpt] == 0)
		return "RANDOM";

	g_StrBuffer[0] = g_BonusData[g_BonusOpt] + 0x20;
	g_StrBuffer[1] = 0;

	return g_StrBuffer;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Wall(u8 op, i8 value)
{
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		g_WallOpt++;
		if (g_WallOpt >= sizeof(g_WallData))
			g_WallOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_WallOpt--;
		if (g_WallOpt == 255)
			g_WallOpt = sizeof(g_WallData) - 1;
		break;
	}

	if (g_WallData[g_WallOpt] == 0)
		return "RANDOM";

	g_StrBuffer[0] = g_WallData[g_WallOpt] + 0x20;
	g_StrBuffer[1] = 0;

	return g_StrBuffer;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Port(u8 op, i8 value)
{
	op;
	u8 tap = NTAP_TYPE_NONE;
	if (value == 0) // port 1
		tap = (g_JoyInfo >> 4) & 0x03;
	else if (value == 1)
		tap = (g_JoyInfo >> 6);
	switch(tap)
	{
	case NTAP_TYPE_NONE:    return "JOYSTICK";
	case NTAP_TYPE_NINJA:   return "NINJA TAP";
	case NTAP_TYPE_SHINOBI: return "SHINOBI TAP";
	}
	return "UNKNOW";
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_MSX(u8 op, i8 value)
{
	op;
	value;
	switch(g_VersionMSX)
	{
	case MSXVER_1:  return "MSX 1";
	case MSXVER_2:  return "MSX 2";
	case MSXVER_2P: return "MSX 2+";
	case MSXVER_TR: return "TURBO R";
	}
	return "UNKNOW";
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_VDP(u8 op, i8 value)
{
	op;
	value;
	switch(g_VersionVDP)
	{
	case VDP_VERSION_TMS9918A: return "TMS9918";
	case VDP_VERSION_V9938:    return "V9938";
	case VDP_VERSION_V9958:    return "V9958";
	}
	return "UNKNOW";
}

//-----------------------------------------------------------------------------
//
/*const c8* MenuAction_Save(u8 op, i8 value)
{
	op;
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
	case MENU_ACTION_DEC:
		break;
	}

	return NULL;
}*/

//-----------------------------------------------------------------------------
//
void MenuOpen_Multi()
{
	g_Scroll = 0;

	if(g_GameMode == MODE_TRAINNNG)
		SetGameMode(MODE_GREEDIEST);

	g_StartPage = MENU_MULTI;
}

//-----------------------------------------------------------------------------
//
void MenuOpen_Solo()
{
	// Compute total
	// if(g_TrainLevel > 0)
	// {
	// 	g_TrainTotal = GetTotalTrainingScore(g_TrainScore, g_TrainLevel - 1);
	// 	g_HiTotal = GetTotalTrainingScore(g_HiScore, g_TrainLevel - 1);
	// }
	// else
	// {
	// 	g_TrainTotal = 0;
	// 	g_HiTotal = 0;
	// }

	if(g_TrainLevel)
	{
		g_MenuSolo[1].Type = MENU_ITEM_ACTION;
		g_MenuSolo[2].Type = MENU_ITEM_INT;
	}
	else
	{
		g_MenuSolo[1].Type = MENU_ITEM_ACTION|MENU_ITEM_DISABLE;
		g_MenuSolo[2].Type = MENU_ITEM_INT|MENU_ITEM_DISABLE;
	}
	
	g_MenuLevelMinMax.Min = 1;
	g_MenuLevelMinMax.Max = g_Cheat ? TRAIN_LEVEL_MAX : g_HiLevel;
	g_MenuLevelMinMax.Step = 1;

	g_StartPage = MENU_SOLO;
}

//-----------------------------------------------------------------------------
//
void MenuOpen_Credit()
{
	g_Scroll = 0;
}

//-----------------------------------------------------------------------------
//
u8 Menu_HandleInput()
{
	u8 new = 0;
	if (IsInputUp())
		new |= MENU_INPUT_UP;
	else if (IsInputDown())
		new |= MENU_INPUT_DOWN;
	if (IsInputLeft())
		new |= MENU_INPUT_LEFT;
	else if (IsInputRight())
		new |= MENU_INPUT_RIGHT;
	if (IsInputButton1())
		new |= MENU_INPUT_TRIGGER;

	u8 ret = (new & g_MenuInputPrev) ^ new;
	g_MenuInputPrev = new;

	return ret;
}

//-----------------------------------------------------------------------------
//
void Menu_HandleDraw(u8 x, u8 y, u8 type, const void* text)
{
	x;
	y;
	type;
	text;
}

//-----------------------------------------------------------------------------
// Callback to handle menu events
void Menu_HandleEvent(u8 event)
{
	switch(event)
	{
	case MENU_EVENT_UP:
	case MENU_EVENT_DOWN:
	case MENU_EVENT_INC:
	case MENU_EVENT_DEC:
		PlaySFX(SFX_MOVE);
		break;
	case MENU_EVENT_SET:
		PlaySFX(SFX_SELECT);
		break;
	}
}

//=============================================================================
// STATES
//=============================================================================

//.............................................................................
// INIT STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Init_Begin()
{
	Bios_SetKeyClick(FALSE);

	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_SetColor(COLOR_BLACK);
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_ClearVRAM();

	// Get VDP version
	if (Keyboard_IsKeyPressed(KEY_1))
		g_VersionVDP = VDP_VERSION_TMS9918A;
	else if (Keyboard_IsKeyPressed(KEY_2))
		g_VersionVDP = VDP_VERSION_V9938;
	else
		g_VersionVDP = VDP_GetVersion();

	// Initialize palette
	if (g_VersionVDP > VDP_VERSION_TMS9918A)
	{
		VDP_SetPalette((u8*)g_MSX2Palette);
		g_PalOpt = PAL_CUSTOM;
	}

	// Initialize frequency
	if (g_VersionROM & 0x80)
		g_FreqDetected = FREQ_50HZ;
	else
		g_FreqDetected = FREQ_60HZ;
	g_Freq = g_FreqDetected;

	// Set VBlank hook
	VDP_EnableVBlank(TRUE);
	Bios_SetHookDirectCallback(H_TIMI, VDP_InterruptHandler);

	// Initialize free device
	loop(i, CTRL_MAX)
		g_CtrlReg[i] = ((i >= g_JoyNum) && (i <= CTRL_JOY_8)) ? CTRL_UNAVAILABLE : CTRL_FREE;

	// Initialize players
	loop(i, PLAYER_MAX)
		InitPlayer(&g_Players[i], i);
	InitPlayer(&g_TrainPlayer, 0);
	g_TrainPlayer.Controller  = CTRL_KEY_1;
	g_TrainPlayer.Action = UpdatePlayerInput;

	// Initialize score
	loop(i, TRAIN_LEVEL_MAX)
	{
		g_TrainScore[i] = 0;
		g_HiScore[i] = 0;
	}
}

//-----------------------------------------------------------------------------
//
void State_Init_Update()
{
	FSM_SetState(&State_Logo);
}


//.............................................................................
// LOGO STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Logo_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_SetColor(COLOR_BLACK);
	VDP_ClearVRAM();

	// Load sprites data
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	Pletter_UnpackToVRAM(g_DataLogoSprt, g_SpritePatternLow + 4 * 8);
	VDP_FillVRAM_16K(0xFF, g_SpritePatternLow, 4 * 8);
	// Set sprites data - Mask
	VDP_SetSpriteSM1(0,  96, 95, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(1, 112, 95, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(2, 128, 95, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(3, 104, 79, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(4, 120, 79, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(5, 104, 63, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(6, 120, 63, 0, COLOR_BLACK);
	// Logo
	VDP_SetSpriteSM1( 7, 104, 63,  4, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1( 8, 120, 63,  8, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1( 9, 104, 79, 12, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(10, 120, 79, 16, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(11, 104, 55, 20, COLOR_DARK_RED);
	VDP_SetSpriteSM1(12, 120, 55, 24, COLOR_DARK_RED);
	VDP_SetSpriteSM1(13, 104, 71, 28, COLOR_WHITE);
	VDP_SetSpriteSM1(14, 120, 71, 32, COLOR_WHITE);
	VDP_SetSpriteSM1(15, 104, 87, 36, COLOR_LIGHT_YELLOW);
	VDP_SetSpriteSM1(16, 120, 87, 40, COLOR_LIGHT_YELLOW);
	VDP_DisableSpritesFrom(17);

	// Load tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataLogoTile_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataLogoTile_Colors, 0);
	// Draw tiles data
	VDP_FillScreen_GM2(0x00);
	VDP_WriteLayout_GM2(g_DataLogoTileL0_Names, 12, 12, 6, 2);
	VDP_WriteLayout_GM2(g_DataLogoTileL1_Names, 13, 8, 4, 4);

	g_Counter = 0;

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_Logo_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if (IsInputButton1())
	{
		PlaySFX(SFX_SELECT);		
		FSM_SetState(&State_Title);
	}

	if ((g_Frame & 0x03) != 0)
		return;

	g_Counter++;
	if ((g_Counter >= LOGO_START) && (g_Counter < LOGO_START + LOGO_OFFSET))
	{
		u8 offset = (95 + LOGO_START) - g_Counter;

		VDP_SetSpritePositionY(0, offset);
		VDP_SetSpritePositionY(1, offset);
		VDP_SetSpritePositionY(2, offset);
		offset -= 16;
		VDP_SetSpritePositionY(3, offset);
		VDP_SetSpritePositionY(4, offset);
		offset -= 16;
		VDP_SetSpritePositionY(5, offset);
		VDP_SetSpritePositionY(6, offset);
	}

	if (g_Counter == LOGO_END)
		FSM_SetState(&State_Title);
}

//.............................................................................
// TITLE MENU STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Title_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_ClearVRAM();

	// Background
	ClearLevel();
	DrawLevel();

	// Initialize sprites data
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_8);
	Pletter_UnpackToVRAM(g_DataSprites, g_SpritePatternLow);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);
	const u8* ptr = g_TitleTile;
	u8 x = 4, y = 3;
	for(u8 j = 0; j < 5; ++j)
	{
		for(u8 i = 0; i < 24; ++i)
		{
			if (*ptr)
				VDP_Poke_GM2(x, y, *ptr);
			++x;
			++ptr;
		}
		++y;
		x = 4;
	}

	VDP_SetColor(COLOR_LIGHT_YELLOW);

	// Draw field
	PrintChr(0,  0, TILE_TREE);
	PrintChr(31, 0, TILE_TREE);
	PrintChrX(1, 0, TILE_TREE, 30);
	PrintChrY(0, 1, TILE_TREE, 22);
	PrintChrY(31,1, TILE_TREE, 22);
	PrintChr(0, 23, TILE_TREE);
	PrintChr(31, 23, TILE_TREE);
	PrintChrX(1, 23, TILE_TREE, 30);

	// Initialize font
	g_PrintData.ScreenWidth = 32;
	g_PrintData.PatternOffset = 0;
	Print_SetFontEx(8, 8, 1, 1, ' ', '_', NULL);
	Print_SetMode(PRINT_MODE_TEXT);
	Print_SetTabSize(3);

	VDP_SetSpriteSM1(0,  7 * 8, 4 * 8 - 1, 7, COLOR_DARK_RED);
	VDP_SetSpriteSM1(1,  7 * 8, 5 * 8 - 1, 7, COLOR_DARK_RED);
	VDP_SetSpriteSM1(2, 11 * 8, 4 * 8 - 1, 3, COLOR_BLACK);
	VDP_SetSpriteSM1(3, 12 * 8, 3 * 8 - 1, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(4, 19 * 8, 3 * 8 - 1, 0, COLOR_BLACK);
	VDP_SetSpriteSM1(5, 20 * 8, 4 * 8 - 1, 1, COLOR_BLACK);
	VDP_SetSpriteSM1(6, 28 * 8, 3 * 8 - 1, 5, COLOR_DARK_RED);
	VDP_DisableSpritesFrom(7);

	Print_DrawTextAt(8, 11, "PIXEL PHENIX 2023");

	if (g_Initialized)
		FSM_SetState(&State_Menu);

	// Initialize music
	PlayMusic(MUSIC_MENU);

	g_Initialized = true;

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void PressKeyBlink()
{
	if (g_Frame & 0x10)
		Print_DrawTextAt(11, 20, "PRESS A KEY");
	else
		VDP_WriteVRAM(g_ScreenBuffer + (20 * 32) + 11, g_ScreenLayoutLow + (20 * 32) + 11, g_ScreenLayoutHigh, 11);
}

//-----------------------------------------------------------------------------
//
void State_Title_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if(g_StartPage != 0xFF)
	{
		FSM_SetState(&State_Menu);
		return;
	}

	PressKeyBlink();

	if (IsInputButton1())
	{
		PlaySFX(SFX_SELECT);
		g_StartPage = MENU_MAIN;	
		FSM_SetState(&State_Menu);
	}
}

//.............................................................................
// MAIN MENU STATE
//.............................................................................

#define MENU_POS_TOP					10
#define MENU_POS_BOTTOM					21
#define MENU_POS_LEFT					3
#define MENU_POS_RIGHT					28

//-----------------------------------------------------------------------------
//
void State_Menu_Begin()
{
	// PlaySFX(SFX_START);

	// Draw menu frame
	PrintChr(MENU_POS_LEFT,    MENU_POS_TOP,    0xE9);
	PrintChrX(MENU_POS_LEFT+1, MENU_POS_TOP,    0xE8, MENU_POS_RIGHT-MENU_POS_LEFT-1);
	PrintChr(MENU_POS_RIGHT,   MENU_POS_TOP,    0xEA);
	PrintChrX(MENU_POS_LEFT+1, MENU_POS_TOP+1,  0x00, MENU_POS_RIGHT-MENU_POS_LEFT-1);
	PrintChrY(MENU_POS_RIGHT,  MENU_POS_TOP+1,  0xEC, MENU_POS_BOTTOM-MENU_POS_TOP-1);
	PrintChr(MENU_POS_RIGHT,   MENU_POS_BOTTOM, 0xEE);
	PrintChrX(MENU_POS_LEFT+1, MENU_POS_BOTTOM, 0xE8, MENU_POS_RIGHT-MENU_POS_LEFT-1);
	PrintChr(MENU_POS_LEFT,    MENU_POS_BOTTOM, 0xED);
	PrintChrY(MENU_POS_LEFT,   MENU_POS_TOP+1,  0xEB, MENU_POS_BOTTOM-MENU_POS_TOP-1);

	// Disable MSX2 only feature
	// if (g_VersionMSX == MSXVER_1)
	// {
	// 	g_MenuMain[5].Type = MENU_ITEM_EMPTY;
	// 	g_MenuOption[2].Type = MENU_ITEM_EMPTY;
	// }

	// Initialize menu
	Menu_Initialize(g_Menus);
	Menu_SetInputCallback(Menu_HandleInput);
	Menu_SetDrawCallback(Menu_HandleDraw);
	Menu_SetEventCallback(Menu_HandleEvent);
	Menu_DrawPage(g_StartPage);
}

//-----------------------------------------------------------------------------
//
void State_Menu_Update()
{
	// Wait V-Synch
	WaitVBlank();

	// Update menu
	Menu_Update();
}

//.............................................................................
// BATTLE SELECT STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
u8 GetPlayerNum()
{
	u8 num = 0;
	loop(i, PLAYER_MAX)
		if(g_Players[i].Controller != CTRL_NONE)
			num++;

	return num;
}

//-----------------------------------------------------------------------------
//
void State_BattleSelect_Begin()
{
	// PlaySFX(SFX_START);
	
	// Initialize VDP
	VDP_EnableDisplay(FALSE);

	//........................................
	// Load tiles

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataSelect_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataSelect_Colors, 0);
	// Portaits tiles
	u16 dst = g_ScreenPatternLow + (160 * 8);
	Pletter_UnpackToVRAM(g_DataFace1_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace1_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace2_Patterns, dst);
	// Portaits colors
	dst = g_ScreenColorLow + (160 * 8);
	Pletter_UnpackToVRAM(g_DataFace1_Colors, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace1_Colors, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace2_Colors, dst);

	//........................................
	// Draw page

	// Background
	VDP_FillScreen_GM2(0x1C);
	// Buttons
	VDP_WriteLayout_GM2(SELECT_START, 2, 1, 7, 2);
	VDP_WriteLayout_GM2(SELECT_EXIT, 24, 1, 6, 2);
	// Frames and devices
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		const Character* info = &g_CharaInfo[i];
		u8 x = info->FrameX;
		u8 y = info->FrameY;
		VDP_WriteLayout_GM2(SELECT_FRAME, x, y, 7, 6);
		VDP_WriteLayout_GM2(info->Face, x + 1, y + 1, 5, 5);
		u8 ctrl = g_Players[i].Controller;
		VDP_WriteLayout_GM2(g_DeviceSelect[ctrl].Default, x, y + 6, 7, 3);
	}

	//........................................
	// Eyes
	u8 sprtId = 4;
	loop(i, 8)
	{
		const Character* info = &g_CharaInfo[i];
		u8 x = info->FrameX * 8 + info->EyeOffset.X;
		u8 y = info->FrameY * 8 + info->EyeOffset.Y;
		VDP_SetSpriteSM1(sprtId++, x, y, info->EyePattern, info->EyeColor);		
	}
	VDP_DisableSpritesFrom(12);

	g_SelectEdit = FALSE;
	MoveCursor(8);

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_BattleSelect_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if (g_SelectEdit) // Handle character edit
	{
		Player* ply = &g_Players[g_SlotIdx];
		if (IsInputLeft())
		{
			PlaySFX(SFX_MOVE);
			SetPrevPlayerController(ply);
			EditPlayer(g_SlotIdx, TRUE);
		}
		else if (IsInputRight())
		{
			PlaySFX(SFX_MOVE);
			SetNextPlayerController(ply);
			EditPlayer(g_SlotIdx, TRUE);
		}
		else if ((g_CtrlTurn == TURN_CUSTOM) && (IsInputUp() || IsInputDown()))
		{
			PlaySFX(SFX_MOVE);
			ply->Turn = 1 - ply->Turn;
			EditPlayer(g_SlotIdx, TRUE);
		}

		if (IsInputButton1())
		{
			PlaySFX(SFX_SELECT);
			EditPlayer(g_SlotIdx, FALSE);
		}
	}
	else // Handle menu update
	{
		// Move cursor
		const SelectSlot* slot = &g_BattleSelectSlot[g_SlotIdx];
		i8 newSlot = -1;
		if (IsInputLeft())
			newSlot = slot->Left;
		else if (IsInputRight())
			newSlot = slot->Right;
		if (IsInputUp())
			newSlot = slot->Up;
		else if (IsInputDown())
			newSlot = slot->Down;
		if (newSlot != -1)
		{
			PlaySFX(SFX_MOVE);
			MoveCursor(newSlot);
		}

		// Animate cursor
		slot = &g_BattleSelectSlot[g_SlotIdx];
		u8 offset = g_CursorAnim[(g_Frame >> 2) % 8];
		VDP_SetSpritePosition(0, slot->UL.X - offset, slot->UL.Y - offset);
		VDP_SetSpritePosition(1, slot->DR.X + offset, slot->UL.Y - offset);
		VDP_SetSpritePosition(2, slot->UL.X - offset, slot->DR.Y + offset);
		VDP_SetSpritePosition(3, slot->DR.X + offset, slot->DR.Y + offset);

		// Eyes blink
		if ((g_Frame & 0x07) == 0x00)
		{
			u8 rnd = Math_GetRandom8();
			u8 sprtId = 4;
			loop(i, 8)
			{
				const Character* info = &g_CharaInfo[i];
				u8 y = (rnd == i) ? VDP_SPRITE_HIDE : info->FrameY * 8 + info->EyeOffset.Y;
				VDP_SetSpritePositionY(sprtId++, y);
			}
		}

		// Handle validation
		if (IsInputButton1())
		{
			PlaySFX(SFX_SELECT);
			switch(g_SlotIdx)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				EditPlayer(g_SlotIdx, TRUE);
				break;
			case 8:
				if(GetPlayerNum() >= 2)
					FSM_SetState(&State_BattleStart);
				else
					PlaySFX(SFX_DEATH);
				return;
			case 9:
				FSM_SetState(&State_Title);
				return;
			};
		}

		if (Keyboard_IsKeyPushed(KEY_RET))
		{
			if(GetPlayerNum() >= 2)
			{
				PlaySFX(SFX_SELECT);
				FSM_SetState(&State_BattleStart);
			}
			else
				PlaySFX(SFX_DEATH);
			return;
		}
		if (Keyboard_IsKeyPushed(KEY_ESC))
		{
			PlaySFX(SFX_SELECT);
			FSM_SetState(&State_Title);
			return;
		}

		// Handle special keys
		loop(i, numberof(g_CtrlBind))
		{
			if(g_CtrlBind[i].bAll && Keyboard_IsKeyPressed(KEY_CTRL))
			{
				if (Keyboard_IsKeyPushed(g_CtrlBind[i].Key))
				{
					loop(j, PLAYER_MAX)
					{
						SetPlayerController(&g_Players[j], g_CtrlBind[i].Ctrl);
						EditPlayer(j, FALSE);
					}
					break;
				}
			}
			else if (g_SlotIdx < 8)
			{
				if (g_CtrlReg[g_CtrlBind[i].Ctrl] == CTRL_FREE)
				{
					if (Keyboard_IsKeyPushed(g_CtrlBind[i].Key))
					{
						SetPlayerController(&g_Players[g_SlotIdx], g_CtrlBind[i].Ctrl);
						EditPlayer(g_SlotIdx, FALSE);
						break;
					}
				}
			}
		}
	}
}

//.............................................................................
// BATTLE START STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_BattleStart_Begin()
{
	// PlaySFX(SFX_START);

	VDP_EnableDisplay(FALSE);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawTile(0,  1, 0xE9);
	DrawTileX(1,  1, 0xE8, 30);
	DrawTile(31, 1, 0xEA);
	DrawTileY(31, 2, 0xEC, 21);
	DrawTile(31, 23, 0xEE);
	DrawTileX(1, 23, 0xE8, 30);
	DrawTile(0, 23, 0xED);
	DrawTileY(0,  2, 0xEB, 21);

	// Timer board
	if (g_ModeInfo[g_GameMode].Time)
	{
		DrawTile(14, 23, TILE_CLOCK + 0);
		DrawTile(15, 23, TILE_CLOCK + 1);
		DrawTile(16, 23, TILE_CLOCK + 2);
		DrawTile(17, 23, TILE_CLOCK + 3);
	}

	// Draw score board
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		DrawTile(i * 4, 0, 0x42 + g_CharaInfo[i].TileBase);
	}
	// Draw pre-hole
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		const Start* start = &g_Starts[i];
		DrawTile(start->X, start->Y, TILE_PREHOLE);
	}

	// Initialize obstacles
	for(u8 i = 0; i < g_WallNum; ++i)
	{
		u8 x = 0, y = 0;
		bool bLoop = TRUE;
		while(bLoop)
		{
			bLoop = FALSE;
			x = Math_GetRandom8() % 32;
			y = 2 + (Math_GetRandom8() % 21);
			// No obstacle on non-empty tile
			if (GetTile(x, y) != TILE_EMPTY)
				bLoop = TRUE;
			// No obstacle in hole lines
			for(u8 j = 0; j < PLAYER_MAX; ++j)
			{
				if (x == g_Starts[j].X)
				{
					bLoop = TRUE;
					break;
				}
				if (y == g_Starts[j].Y)
				{
					bLoop = TRUE;
					break;
				}
			}
		}
		DrawTile(x, y, GetWallTile());
	}

	// Copy screen buffer to VRAM
	DrawLevel();

	// Spawn players
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		ResetPlayer(&g_Players[i]);
		VDP_HideSprite(i);
	}
	// Initialize sprites data
	VDP_DisableSpritesFrom(8);

	g_CurrentPlayer = 0;
	g_DoSynch = (GetHumanCount() > 0);
	g_Counter = 0;

	PlayMusic(MUSIC_BATTLE);

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_BattleStart_Update()
{
	// Wait V-Synch
	WaitVBlank();

	// Check input
	if (IsInputButton1())
	{
		PlaySFX(SFX_SELECT);		
		FSM_SetState(&State_BattleGame);
		return;
	}
	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);		
		FSM_SetState(&State_BattleSelect);
		return;
	}

	// Skip start sequence
	if ((g_Frame & 0x0F) != 0)
		return;

	// Display character and control
	g_Counter++;

	if (g_Counter & 1)
	{
		for(u8 i = 0; i < PLAYER_MAX; ++i)
		{
			const Player* ply = &g_Players[i];
			if (ply->Controller != CTRL_NONE)
			{
				u8 x = ply->PosX;
				u8 y = ply->PosY;
				PrintChr(x, y, TILE_PREHOLE);
				PrintChr(++x, y, TILE_EMPTY);
			}
			VDP_HideSprite(i);
		}
	}
	else
	{
		for(u8 i = 0; i < PLAYER_MAX; ++i)
		{
			const Player* ply = &g_Players[i];

			u8 num = 0xFF;
			u8 tile = TILE_BONUS;
			switch(ply->Controller)
			{
			case CTRL_JOY_1:
			case CTRL_JOY_2:
			case CTRL_JOY_3:
			case CTRL_JOY_4:
			case CTRL_JOY_5:
			case CTRL_JOY_6:
			case CTRL_JOY_7:
			case CTRL_JOY_8:
				num = 21 - 4 + ply->Controller;
				tile = TILE_JOY;
				break;
			case CTRL_KEY_1:
				tile = TILE_KB1;
				break;
			case CTRL_KEY_2:
				tile = TILE_KB2;
				break;
			case CTRL_AI_EASY:
			case CTRL_AI_MED:
			case CTRL_AI_HARD:
				tile = 0;
				break;
			case CTRL_NONE:
				continue;
			}

			u8 x = ply->PosX;
			u8 y = ply->PosY;
			PrintChr(x, y, 0x42 + g_CharaInfo[i].TileBase);
			if (tile)
				PrintChr(++x, y, tile);
			if (num != 0xFF)
				VDP_SetSpriteSM1(i, x * 8 + 9, y * 8 + 1, num, COLOR_BLACK);
		}
	}

	// Time counter
	u8 step = g_Counter >> 2;
	switch(step)
	{
	case 0: // 5
	case 1: // 4
	case 2: // 3
	case 3: // 2
	case 4: // 1
		DrawCounter(14, 10, step);
		break;
	case 5: // GO
		PrintChrY(15, 10, TILE_EMPTY, 5);
		DrawCounter(12, 10, 5);
		DrawCounter(16, 10, 6);
		break;
	case 6:
		FSM_SetState(&State_BattleGame);
		return;
	}
}

//.............................................................................
// BATTLE GAME STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_BattleGame_Begin()
{
	// Initialize sprites data
	for(u8 i = 0; i < 8; ++i)
		VDP_HideSprite(i);
	VDP_DisableSpritesFrom(8);

	// Initialize timer
	if (g_ModeInfo[g_GameMode].Time)
	{
		if(g_GameMode == MODE_BATTLEROYAL)
			SetTimer(BATTLEROYAL_TIME);
		else
			SetTimer(g_GameCount);
	}
	g_CollapsePhase = COLLAPSE_OFF;

	// Copy screen buffer to VRAM
	DrawLevel();

	// Draw score
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		if (g_Players[i].Controller != CTRL_NONE)
			SetScore(&g_Players[i]);

	// Draw pre-hole
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		const Start* start = &g_Starts[i];
		PrintChr(start->X, start->Y, TILE_PREHOLE);
	}

	// Initialize Bonus
	SpawnBonus();
}

void UpdateInput()
{
	// Update keyboard entries
	if (g_Input[CTRL_KEY_1] == DIR_MAX)
	{
		if (Keyboard_IsKeyPushed(KEY_LEFT))
			g_Input[CTRL_KEY_1] = DIR_LEFT;
		else if (Keyboard_IsKeyPushed(KEY_RIGHT))
			g_Input[CTRL_KEY_1] = DIR_RIGHT;
		else if (Keyboard_IsKeyPushed(KEY_UP))
			g_Input[CTRL_KEY_1] = DIR_UP;
		else if (Keyboard_IsKeyPushed(KEY_DOWN))
			g_Input[CTRL_KEY_1] = DIR_DOWN;
	}
	if (g_Input[CTRL_KEY_2] == DIR_MAX)
	{
		if (Keyboard_IsKeyPushed(KEY_D))
			g_Input[CTRL_KEY_2] = DIR_LEFT;
		else if (Keyboard_IsKeyPushed(KEY_G))
			g_Input[CTRL_KEY_2] = DIR_RIGHT;
		else if (Keyboard_IsKeyPushed(KEY_R))
			g_Input[CTRL_KEY_2] = DIR_UP;
		else if (Keyboard_IsKeyPushed(KEY_F))
			g_Input[CTRL_KEY_2] = DIR_DOWN;
	}
	// Update joysticks
	for(u8 i = 0; i < g_JoyNum; ++i)
	{
		if (g_Input[i] == DIR_MAX)
		{
			if (NTap_IsPushed(i, NTAP_LEFT))
				g_Input[i] = DIR_LEFT;
			else if (NTap_IsPushed(i, NTAP_RIGHT))
				g_Input[i] = DIR_RIGHT;
			else if (NTap_IsPushed(i, NTAP_UP))
				g_Input[i] = DIR_UP;
			else if (NTap_IsPushed(i, NTAP_DOWN))
				g_Input[i] = DIR_DOWN;
		}
	}
}

//-----------------------------------------------------------------------------
//
void State_BattleGame_Update()
{
	// Wait V-Synch
	if (g_DoSynch)
		WaitVBlank();
	else
		g_Frame++;

	// Update one of the players
	Player* ply = &g_Players[g_CurrentPlayer];
	UpdatePlayer(ply);
	g_CurrentPlayer++;
	g_CurrentPlayer %= PLAYER_MAX;

	UpdateInput();

	if (g_ModeInfo[g_GameMode].Time && (g_CollapsePhase != COLLAPSE_OFF)) // Field is collapsing...
	{
		if ((g_CollapseTimer >= 32) && (g_CollapseY0 < 12))
		{
			u8 phase = g_CollapsePhase & 0x3;
			u8 tile = g_HoleAnim[phase];

			PrintChrX(g_CollapseX0, g_CollapseY0, tile, g_CollapseX1 - g_CollapseX0 + 1);
			PrintChrX(g_CollapseX0, g_CollapseY1, tile, g_CollapseX1 - g_CollapseX0 + 1);
			PrintChrY(g_CollapseX0, g_CollapseY0 + 1, tile, g_CollapseY1 - g_CollapseY0 - 1);
			PrintChrY(g_CollapseX1, g_CollapseY0 + 1, tile, g_CollapseY1 - g_CollapseY0 - 1);

			if (phase == 3)
			{
				g_CollapseX0++;
				g_CollapseY0++;
				g_CollapseX1--;
				g_CollapseY1--;
			}

			g_CollapseTimer = 0;
			g_CollapsePhase++;
		}
		g_CollapseTimer++;
	}
	else
	{
		VDP_Poke_GM2(g_BonusPos.X, g_BonusPos.Y, g_BonusTile);

		if (g_ModeInfo[g_GameMode].Time)
		{
			bool bZero = !UpdateTimer();
			if (bZero)
			{
				switch(g_GameMode)
				{
				case MODE_SIZEMATTER:
					CheckSizeMatter();
					break;

				default:
					g_CollapsePhase = 0;
					g_CollapseTimer = 0;
					g_CollapseX0 = 1;
					g_CollapseY0 = 2;
					g_CollapseX1 = 30;
					g_CollapseY1 = 22;
					break;
				}
			}
		}
	}

	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);		
		FSM_SetState(&State_BattleSelect);
	}
}

//.............................................................................
// VICTORY STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Victory_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);

	// Initialize tiles data
	// VDP_FillLayout_GM2(TILE_EMPTY, 0, 0, 32, 24);
	ClearLevel();
	// Draw score board
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		DrawTile(i * 4, 0, 0x42 + g_CharaInfo[i].TileBase);
	DrawLevel();

	// Draw score
	if(g_GameMode != MODE_TRAINNNG)
	{
		for(u8 i = 0; i < PLAYER_MAX; ++i)
			if (g_Players[i].Controller != CTRL_NONE)
				SetScore(&g_Players[i]);
	}

	// Draw field
	PrintChr(0,  1, TILE_TREE);
	PrintChrX(1, 1, TILE_TREE, 30);
	PrintChr(31, 1, TILE_TREE);
	PrintChrY(31,2, TILE_TREE, 22);
	PrintChr(31, 23, TILE_TREE);
	PrintChrX(1, 23, TILE_TREE, 30);
	PrintChr(0, 23, TILE_TREE);
	PrintChrY(0, 2, TILE_TREE, 22);

	// Text W
	PrintChrY(7, 9, TILE_BALL, 4);
	PrintChr(8, 13, TILE_BALL);
	PrintChr(9, 12, TILE_BALL);
	PrintChr(10, 13, TILE_BALL);
	PrintChrY(11, 9, TILE_BALL, 4);
	// Text I
	PrintChrX(13, 9, TILE_BALL, 3);
	PrintChrY(14, 10, TILE_BALL, 3);
	PrintChrX(13, 13, TILE_BALL, 3);
	// Text N
	PrintChrY(17, 9, TILE_BALL, 5);
	PrintChr(18, 10, TILE_BALL);
	PrintChr(19, 11, TILE_BALL);
	PrintChr(20, 12, TILE_BALL);
	PrintChrY(21, 9, TILE_BALL, 5);
	// Text !
	PrintChrY(24, 9, TILE_BALL, 3);
	PrintChr(24, 13, TILE_BALL);

	// Setup sprite
	VDP_HideSprite(0);
	VDP_HideSprite(1);
	VDP_HideSprite(2);
	VDP_HideSprite(3);
	VDP_DisableSpritesFrom(4);

	Player* ply = g_Winner;
	ply->PosX = 7;
	ply->PosY = 15;
	ply->Dir = DIR_RIGHT;
	ply->Expect = CLAMP8(ply->Length, 3, 50);
	ply->Length = 1;

	VDP_EnableDisplay(TRUE);

	PlayMusic(MUSIC_VICTORY);
}

//-----------------------------------------------------------------------------
//
void State_Victory_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if (IsInputButton1() || IsInputButton2())
	{
		PlaySFX(SFX_SELECT);		
		if (g_GameMode == MODE_TRAINNNG)
			FSM_SetState(&State_Title);
		else
			FSM_SetState(&State_BattleSelect);
		PlayMusic(MUSIC_MENU);
		return;
	}

	u8 col = (g_Frame & 0x08) ? 0 : 2;
	VDP_LoadColor_GM2(g_BallColor[col], 1, TILE_BALL);

	if (g_Frame & 0x10)
		Print_DrawTextAt(11, 20, "PRESS SPACE");
	else
		VDP_WriteVRAM(g_ScreenBuffer + (20 * 32) + 11, g_ScreenLayoutLow + (20 * 32) + 11, g_ScreenLayoutHigh, 11);

	if ((g_Frame & 0x07) != 0)
		return;


	// Move
	Player* ply = g_Winner;
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 nextDir = ply->Dir;
	switch(ply->Dir)
	{
	case DIR_UP:
		y--;
		if (y <= 7)
			nextDir = DIR_LEFT;
		break;
	case DIR_RIGHT:
		x++;
		if (x >= 26)
			nextDir = DIR_UP;
		break;
	case DIR_DOWN:
		y++;
		if (y >= 15)
			nextDir = DIR_RIGHT;
		break;
	case DIR_LEFT:
		x--;
		if (x <= 5)
			nextDir = DIR_DOWN;
		break;
	}

	// Move
	DrawPlayer(ply, x, y);
	// Turn if needed
	ply->Dir = nextDir;
}

//.............................................................................
// TRAINING SELECT STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void UnpackCrawler(u8 id)
{
	u16 dst = g_ScreenPatternLow + 0x800 + (160 * 8);
	Pletter_UnpackToVRAM(id < 4 ? g_DataFace1_Patterns : g_DataFace2_Patterns, dst);
	dst = g_ScreenColorLow + 0x800 + (160 * 8);
	Pletter_UnpackToVRAM(id < 4 ? g_DataFace1_Colors : g_DataFace2_Colors, dst);
}

//-----------------------------------------------------------------------------
//
void SelectCrawler(u8 id)
{
	u8 prev = g_TrainPlayer.ID;
	g_TrainPlayer.ID = id;

	// Clean
	VDP_FillLayout_GM2(0x1C, TRAIN_FRAME_X + 1, TRAIN_FRAME_Y + 1, 5, 5);
	VDP_HideSprite(0); // Eyes

	// Load data
	if(g_VersionVDP == VDP_VERSION_TMS9918A)
	{
		if((prev / 4) != (id / 4))
		{
			VDP_EnableDisplay(FALSE);
			UnpackCrawler(id); // On MSX 1, unpack is too fast when screen is enable
			VDP_EnableDisplay(TRUE);
		}
	}
	else
		UnpackCrawler(id);

	// Draw face
	const Character* info = &g_CharaInfo[id];
	VDP_WriteLayout_GM2(info->Face, TRAIN_FRAME_X + 1, TRAIN_FRAME_Y + 1, 5, 5);

	// Eye sprite
	u8 x = TRAIN_FRAME_X * 8 + info->EyeOffset.X;
	u8 y = TRAIN_FRAME_Y * 8 + info->EyeOffset.Y;
	VDP_SetSpriteSM1(0, x, y, info->EyePattern, info->EyeColor); // Eyes
}

//-----------------------------------------------------------------------------
//
void SelectControl(u8 id)
{
	VDP_WriteLayout_GM2(g_DeviceSelect[id].Edit, TRAIN_FRAME_X, TRAIN_FRAME_Y + 6, 7, 3);
	g_TrainPlayer.Controller = id;
}

//-----------------------------------------------------------------------------
//
void State_TrainSelect_Begin()
{
	// PlaySFX(SFX_START);

	VDP_EnableDisplay(FALSE);

	//........................................
	// Load tiles

	// Initialize tiles pattern
	u16 dst = g_ScreenPatternLow + (0 * 8);
	Pletter_UnpackToVRAM(g_DataTiles_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataSelect_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataSelect_Patterns, dst);

	// Initialize tiles color
	dst = g_ScreenColorLow + (0 * 8);
	Pletter_UnpackToVRAM(g_DataTiles_Colors, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataSelect_Colors, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataSelect_Colors, dst);

	//........................................
	// Draw page

	// Background
	VDP_FillVRAM(0x00, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32*8);
	VDP_FillVRAM(0x1C, g_ScreenLayoutLow + 32*8, g_ScreenLayoutHigh, 32*16);

	// Text
	Print_DrawTextAt(9, 6, "SELECT CRAWLER");

	// Display player character
	VDP_WriteLayout_GM2(SELECT_FRAME, TRAIN_FRAME_X, TRAIN_FRAME_Y, 7, 6);
	VDP_WriteLayout_GM2(SELECT_CHARA, TRAIN_FRAME_X, TRAIN_FRAME_Y + 6, 7, 3);
	u8 id = g_TrainPlayer.ID;
	if(!g_Continue)
		id = Math_GetRandom8() % PLAYER_MAX;
	UnpackCrawler(id); // Force to unpack once for MSX1
	SelectCrawler(id);
	g_SelectEdit = FALSE;

	// Sprite
	VDP_DisableSpritesFrom(1);

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_TrainSelect_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if(g_SelectEdit)
	{
		if (IsInputRight())
		{
			PlaySFX(SFX_MOVE);
			u8 ctrl = (g_TrainPlayer.Controller + 1) % CTRL_PLY_NUM;
			if((ctrl >= g_JoyNum) && (ctrl < CTRL_KEY_1))
				ctrl = CTRL_KEY_1;
			SelectControl(ctrl);
		}
		else if (IsInputLeft())
		{
			PlaySFX(SFX_MOVE);
			u8 ctrl = (g_TrainPlayer.Controller + CTRL_PLY_NUM - 1) % CTRL_PLY_NUM;
			if((ctrl >= g_JoyNum) && (ctrl < CTRL_KEY_1))
				ctrl = g_JoyNum - 1;
			SelectControl(ctrl);
		}

		if (IsInputButton1())
		{
			PlaySFX(SFX_SELECT);
			if(!g_Continue)
			{
				g_TrainLevel = 1;
				g_HiLevel = 1;
			}
			FSM_SetState(&State_TrainGame);
		}
	}
	else
	{
		if (IsInputRight())
		{
			PlaySFX(SFX_MOVE);
			u8 id = (g_TrainPlayer.ID + 1) % PLAYER_MAX;
			SelectCrawler(id);
		}
		else if (IsInputLeft())
		{
			PlaySFX(SFX_MOVE);
			u8 id = (g_TrainPlayer.ID + PLAYER_MAX - 1) % PLAYER_MAX;
			SelectCrawler(id);
		}

		if (IsInputButton1())
		{
			PlaySFX(SFX_SELECT);
			g_SelectEdit = TRUE;
			Print_DrawTextAt(9, 6, "SELECT CONTROL");
			SelectControl(g_TrainPlayer.Controller);
		}
	}

	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}
}

//.............................................................................
// TRAINING GAME STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_TrainGame_Begin()
{
	VDP_EnableDisplay(FALSE);

	VDP_HideAllSprites();
	// VDP_DisableSpritesFrom(0);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawTile(0,  1, 0xE9);
	DrawTileX(1,  1, 0xE8, 30);
	DrawTile(31, 1, 0xEA);
	DrawTileY(31, 2, 0xEC, 21);
	DrawTile(31, 23, 0xEE);
	DrawTileX(1, 23, 0xE8, 30);
	DrawTile(0, 23, 0xED);
	DrawTileY(0,  2, 0xEB, 21);

	// Timer board
	DrawTile(14, 23, TILE_CLOCK + 0);
	DrawTile(15, 23, TILE_CLOCK + 1);
	DrawTile(16, 23, TILE_CLOCK + 2);
	DrawTile(17, 23, TILE_CLOCK + 3);

	// Draw score board
	Player* ply = &g_TrainPlayer;
	DrawTile(1, 0, 0x42 + g_CharaInfo[ply->ID].TileBase);
	UnpackTrainField(g_TrainLevel - 1);
	DrawLevel();
	
	// Initialize font
	g_PrintData.ScreenWidth = 32;
	g_PrintData.PatternOffset = 0;
	Print_SetFontEx(8, 8, 1, 1, ' ', '_', NULL);
	Print_SetMode(PRINT_MODE_TEXT);
	Print_SetTabSize(3);

	// Draw info
	Print_DrawTextAt(4, 0, "LEVEL:");
	Print_DrawInt(g_TrainLevel);

	Print_DrawTextAt(14, 0, "SCORE:");
	Print_DrawInt(g_TrainTotal);

	// Init player
	g_GameMode = MODE_TRAINNNG;
	g_BonusLen = TRAIN_GROWTH;
	SpawnPlayer(ply);
	ply->PosX = g_PlayerStart.X;
	ply->PosY = g_PlayerStart.Y;
	ply->Dir = DIR_UP;
	ply->Score = 0;
	
	SetTimer(0);
	PlayMusic(g_TrainLevel & 1 ? MUSIC_BATTLE : MUSIC_HURRYUP);

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_TrainGame_Update()
{
	// Wait V-Synch
	WaitVBlank();

	UpdateInput();

	if (g_Cheat && Keyboard_IsKeyPushed(KEY_N))
	{
		FSM_SetState(&State_TrainScore);
		return;
	}
	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}

	// Timer
	Player* ply = &g_TrainPlayer;
	if(ply->State == STATE_PLAYING)
		IncTimer();

	// Update only every 8th frame
	g_CurrentPlayer++;
	u8 speed = (ply->State == STATE_PLAYING) ? g_Speed : SPEED_NORMAL;
	if(g_CurrentPlayer != g_SpeedData[speed].Count)
		return;
	
	g_CurrentPlayer = 0;

	// Update one of the players
	UpdatePlayer(ply);
	if(ply->State == STATE_PLAYING)
		ply->Score++;
}

//.............................................................................
// TRAINING SCORE STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_TrainScore_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_DisableSpritesFrom(0);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawLevel();

	// Draw field
	PrintChr(0,  0, TILE_TREE);
	PrintChrX(1, 0, TILE_TREE, 30);
	PrintChrX(1, 6, TILE_TREE, 30);
	PrintChr(31, 0, TILE_TREE);
	PrintChrY(31,1, TILE_TREE, 22);
	PrintChr(31, 23, TILE_TREE);
	PrintChrX(1, 23, TILE_TREE, 30);
	PrintChr(0, 23, TILE_TREE);
	PrintChrY(0, 1, TILE_TREE, 22);

	// Compute level score
	Player* ply = &g_TrainPlayer;
	u16 score = 0;
	u16 maxScore = g_SpeedData[g_Speed].Score;
	if (ply->Score < maxScore)
		score = maxScore - ply->Score;
	if (score > g_HiScore[g_TrainLevel - 1])
		g_HiScore[g_TrainLevel - 1] = score;
	g_TrainScore[g_TrainLevel - 1] = score;

	// Compute total
	g_TrainTotal = GetTotalTrainingScore(g_TrainScore, g_TrainLevel - 1);
	g_HiTotal = GetTotalTrainingScore(g_HiScore, g_TrainLevel - 1);

	Print_DrawTextAt(9, 3, "LEVEL ");
	Print_DrawInt(g_TrainLevel);
	Print_DrawText(" CLEAR!");

	Print_DrawTextAt(11, 10, "SCORE:");
	Print_DrawIntAt(17, 10, g_TrainScore[g_TrainLevel - 1]);
	Print_DrawTextAt(11, 11, "TOTAL:");
	Print_DrawIntAt(17, 11, g_TrainTotal);

	Print_DrawTextAt(8, 14, "HI-SCORE:");
	Print_DrawIntAt(17, 14, g_HiScore[g_TrainLevel - 1]);
	Print_DrawTextAt(11, 15, "TOTAL:");
	Print_DrawIntAt(17, 15, g_HiTotal);

	// Move to next level
	g_Winner = NULL;
	if(g_TrainLevel < TRAIN_LEVEL_MAX)
		g_TrainLevel++;
	else
		g_Winner = &g_TrainPlayer;
	if (g_TrainLevel > g_HiLevel)
		g_HiLevel = g_TrainLevel;

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_TrainScore_Update()
{
	// Wait V-Synch
	WaitVBlank();

	PressKeyBlink();

	// Handle input
	if (IsInputButton1())
	{
		PlaySFX(SFX_MOVE);
		if (g_Winner)
			FSM_SetState(&State_Victory);
		else
			FSM_SetState(&State_TrainGame);
	}
	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}
}

//=============================================================================
// MAIN
//=============================================================================

//-----------------------------------------------------------------------------
// Check if cheat is activated or no
void CheckCheat()
{
	if(g_Cheat)
		return;
	if(!Keyboard_IsKeyPressed(KEY_N))
		return;
	if(!Keyboard_IsKeyPressed(KEY_O))
		return;
	if(!Keyboard_IsKeyPressed(KEY_E))
		return;

	PlaySFX(SFX_VICTORY);
	g_Cheat = TRUE;
	g_HiLevel = TRAIN_LEVEL_MAX;
}

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	// Initialize screen
	VDP_SetColor(COLOR_BLACK);
	VDP_EnableDisplay(FALSE);

	// Initialize empty music
	PlayMusic(MUSIC_EMPTY);

	// Initialize Joystick and/or Ninja Tap
	NTap_Check();
	g_JoyInfo = NTap_GetInfo();
	g_JoyNum = NTap_GetPortNum();
	g_PlayerMax = MIN(g_JoyNum + 2, 8);

	// Start the Final State Machine
	FSM_SetState(&State_Init);
	while(1)
	{
		Keyboard_Update();
		CheckCheat();
		NTap_Update();
		FSM_Update();
	}

	Bios_Exit(0);
}