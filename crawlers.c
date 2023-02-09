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

void State_Select_Begin();
void State_Select_Update();

void State_Start_Begin();
void State_Start_Update();

void State_Game_Begin();
void State_Game_Update();

void State_Victory_Begin();
void State_Victory_Update();

const c8* MenuAction_Start(u8 op, i8 value);
const c8* MenuAction_Mode(u8 op, i8 value);
const c8* MenuAction_Info(u8 op, i8 value);
const c8* MenuAction_Freq(u8 op, i8 value);
const c8* MenuAction_Palette(u8 op, i8 value);
const c8* MenuAction_Music(u8 op, i8 value);
const c8* MenuAction_Bonus(u8 op, i8 value);
const c8* MenuAction_Wall(u8 op, i8 value);
const c8* MenuAction_Port(u8 op, i8 value);
const c8* MenuAction_MSX(u8 op, i8 value);
const c8* MenuAction_VDP(u8 op, i8 value);
const c8* MenuAction_Exit(u8 op, i8 value);

void InitPlayer(Player* ply, u8 id);
void ResetPlayer(Player* ply);
void SpawnPlayer(Player* ply);
void DrawPlayer(Player* ply);
void ClearPlayer(Player* ply);
void UpdatePlayer(Player* ply);

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Tiles
#include "content/tiles.h"
#include "content/logo_tile.h"

// Sprites
#include "content/sprites.h"
#include "content/logo_sprt.h"
#include "content/eyes.h"

// Menu
#include "content/select.h"
#include "content/face1.h"
#include "content/face2.h"

// Music
#include "content/music_main.h"

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
	{ "Chara 01", 0*20, 0   , 2,    6,  SELECT_FACE_1 },
	{ "Chara 02", 2*20, 1   , 2+7,  6,  SELECT_FACE_2 },
	{ "Chara 03", 6*20, 0xFF, 2+14, 6,  SELECT_FACE_3 },
	{ "Chara 04", 5*20, 0xFF, 2+21, 6,  SELECT_FACE_4 },
	{ "Chara 05", 7*20, 0xFF, 2,    15, SELECT_FACE_5 },
	{ "Chara 06", 4*20, 0xFF, 2+7,  15, SELECT_FACE_6 },
	{ "Chara 07", 1*20, 2   , 2+14, 15, SELECT_FACE_7 },
	{ "Chara 08", 3*20, 3   , 2+21, 15, SELECT_FACE_8 },
};

//
const Start g_Starts[PLAYER_MAX] =
{
	{  5,  6, DIR_RIGHT },
	{ 26,  6, DIR_DOWN  },
	{ 26, 18, DIR_LEFT  },
	{  5, 18, DIR_UP    },
	{  9, 10, DIR_DOWN  },
	{ 22, 10, DIR_LEFT  },
	{ 22, 16, DIR_UP    },
	{  9, 16, DIR_RIGHT },
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

//
const c8 g_TitleTile[] =
{
	0xC0, 0xBD, 0xC1, 0x84, 0x80, 0x85, 0x48, 0x45, 0x49, 0xB5, 0x00, 0xA4, 0xDD, 0x00, 0x00, 0x5C, 0x59, 0x66, 0x98, 0x94, 0x99, 0x70, 0x6C, 0x69,
	0xBF, 0x00, 0xBA, 0x82, 0x7F, 0x87, 0x47, 0x00, 0x42, 0xAB, 0x00, 0xAB, 0xD3, 0x00, 0x00, 0x56, 0x00, 0x00, 0x96, 0x93, 0x9B, 0x6E, 0x00, 0x00,
	0xBE, 0x00, 0x00, 0x83, 0x7F, 0x85, 0x46, 0x43, 0x49, 0xAA, 0x00, 0xAA, 0xD2, 0x00, 0x00, 0x5C, 0x55, 0x00, 0x97, 0x93, 0x99, 0x72, 0x6D, 0x71,
	0xBF, 0x00, 0xC9, 0x82, 0x00, 0x83, 0x47, 0x00, 0x47, 0xAB, 0xA4, 0xAB, 0xD3, 0x00, 0x00, 0x5B, 0x00, 0x00, 0x96, 0x00, 0x96, 0x00, 0x00, 0x6F,
	0xC2, 0xBC, 0xC3, 0x89, 0x00, 0x88, 0x4D, 0x00, 0x4C, 0xAE, 0xAF, 0xB1, 0xD6, 0xD0, 0xCD, 0x5E, 0x58, 0x67, 0x9C, 0x00, 0x9D, 0x77, 0x6C, 0x73,
};

const MenuItemMinMax g_MenuRoundsMinMax = { 1, 10, 1 };
const MenuItemMinMax g_MenuTreesMinMax =  { 0, 100, 10 };

//
const MenuItem g_MenuMain[] =
{
#if (EXT_VERSION)
	{ "SOLO MODE",           MENU_ITEM_GOTO, NULL, MENU_SOLO },
#endif
	{ "BATTLE",              MENU_ITEM_GOTO, NULL, MENU_MULTI },
	{ "OPTIONS",             MENU_ITEM_GOTO, NULL, MENU_OPTION },
	{ "INFORMATION",         MENU_ITEM_GOTO, NULL, MENU_SYSTEM },
	{ "CREDITS",             MENU_ITEM_GOTO, NULL, MENU_CREDIT },
#if (TARGET_TYPE != TYPE_ROM)
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "EXIT",                MENU_ITEM_ACTION, MenuAction_Exit, 0 },
#endif
};

#if (EXT_VERSION)
//
const MenuItem g_MenuSolo[] =
{
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};
#endif

//
const MenuItem g_MenuMulti[] =
{
	{ "START",               MENU_ITEM_ACTION, MenuAction_Start, 0 },
	{ "MODE",                MENU_ITEM_ACTION, MenuAction_Mode, 0 },
	{ "ROUNDS",              MENU_ITEM_INT, &g_GameCount, (i16)&g_MenuRoundsMinMax },
	{ "TIME",                MENU_ITEM_INT, &g_TimeMax, (i16)&g_MenuRoundsMinMax },
	{ "WALLS",               MENU_ITEM_INT, &g_WallNum, (i16)&g_MenuTreesMinMax },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_UPDATE, MenuAction_Info, 0 },
};

//
const MenuItem g_MenuOption[] =
{
	{ "FREQ",                MENU_ITEM_ACTION, MenuAction_Freq, 0 },
	{ "PALETTE",             MENU_ITEM_ACTION, MenuAction_Palette, 0 },
	{ "MUSIC",               MENU_ITEM_ACTION, MenuAction_Music, 0 },
	{ "SFX",                 MENU_ITEM_BOOL, &g_OptSFX, 0 },
	{ "BONUS",               MENU_ITEM_ACTION, MenuAction_Bonus, 0 },
	{ "WALL",                MENU_ITEM_ACTION, MenuAction_Wall, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const MenuItem g_MenuSystem[] =
{
	{ "SYSTEM",              MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_MSX, 0 },
	{ "VIDEO",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_VDP, 0 },
	{ "PORT1",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_Port, 0 },
	{ "PORT2",               MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_Port, 1 },
	{ "TOTAL\x1C",           MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_JoyNum, NULL },
	{ "MAX$",                MENU_ITEM_INT|MENU_ITEM_DISABLE, &g_PlayerMax, NULL },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const MenuItem g_MenuCredit[] =
{
	{ "PIXEL PHENIX 2023",   MENU_ITEM_TEXT, NULL, 1 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "POWERED BY " MSXGL,   MENU_ITEM_TEXT, NULL, 2 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "CODE:  AOINEKO",      MENU_ITEM_TEXT, NULL, 0 },
	{ "GRAPH: AOINEKO",      MENU_ITEM_TEXT, NULL, 0 },
	{ "MUSIC: TOTTA",        MENU_ITEM_TEXT, NULL, 0 },
	{ "SFX:   AYFX SAMPLE",  MENU_ITEM_TEXT, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const Menu g_Menus[MENU_MAX] =
{
	{ NULL, g_MenuMain,   numberof(g_MenuMain) },
#if (EXT_VERSION)
	{ NULL, g_MenuSolo,   numberof(g_MenuSolo) },
#endif
	{ NULL, g_MenuMulti,  numberof(g_MenuMulti) },
	{ NULL, g_MenuOption, numberof(g_MenuOption) },
	{ NULL, g_MenuSystem, numberof(g_MenuSystem) },
	{ NULL, g_MenuCredit, numberof(g_MenuCredit) },
};

// 
const FSM_State State_Init =	{ 0, State_Init_Begin,		State_Init_Update,		NULL };
const FSM_State State_Logo =	{ 0, State_Logo_Begin,		State_Logo_Update,		NULL };
const FSM_State State_Title =	{ 0, State_Title_Begin,		State_Title_Update,		NULL };
const FSM_State State_Menu =	{ 0, State_Menu_Begin,		State_Menu_Update,		NULL };
const FSM_State State_Select =	{ 0, State_Select_Begin,	State_Select_Update,	NULL };
const FSM_State State_Start =	{ 0, State_Start_Begin,		State_Start_Update,		NULL };
const FSM_State State_Game =	{ 0, State_Game_Begin,		State_Game_Update,		NULL };
const FSM_State State_Victory =	{ 0, State_Victory_Begin,	State_Victory_Update,	NULL };

// 
const SelectDevice g_Devices[CTRL_MAX] =
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

// Select slots data
const SelectSlot g_SelectSlot[] =
{	//                             L   R   U   D
	{ {  22,  53 }, {  58,  89 }, -1,  1,  8,  4 }, // 0 - Face 1
	{ {  78,  53 }, { 114,  89 },  0,  2,  8,  5 }, // 1 - Face 2
	{ { 134,  53 }, { 170,  89 },  1,  3,  9,  6 }, // 2 - Face 3
	{ { 190,  53 }, { 226,  89 },  2, -1,  9,  7 }, // 3 - Face 4
	{ {  22, 125 }, {  58, 161 }, -1,  5,  0, -1 }, // 4 - Face 5
	{ {  78, 125 }, { 114, 161 },  4,  6,  1, -1 }, // 5 - Face 6
	{ { 134, 125 }, { 170, 161 },  5,  7,  2, -1 }, // 6 - Face 7
	{ { 190, 125 }, { 226, 161 },  6, -1,  3, -1 }, // 7 - Face 8
	{ {  22,  15 }, {  58,  22 }, -1,  9, -1,  0 }, // 8 - Start
	{ { 198,  15 }, { 226,  22 },  8, -1, -1,  3 }, // 9 - Exit
};

// Cursor offset animation
const u8 g_CursorAnim[] = { 1, 1, 2, 1, 1, 0, 0, 0 };

// Hole tile animation
const u8 g_HoleAnim[] = { 0xF3, 0xF4, 0xF5, 0xE3 };

// Controller binding
const CtrlBind g_CtrlBind[] =
{
	{ KEY_DEL,	CTRL_NONE },
	{ KEY_F1,	CTRL_AI_EASY },
	{ KEY_F2,	CTRL_AI_MED },
	{ KEY_F3,	CTRL_AI_HARD },
	{ KEY_F4,	CTRL_KEY_1 },
	{ KEY_F5,	CTRL_KEY_2 },
	{ KEY_1,	CTRL_JOY_1 },
	{ KEY_2,	CTRL_JOY_2 },
	{ KEY_3,	CTRL_JOY_3 },
	{ KEY_4,	CTRL_JOY_4 },
	{ KEY_5,	CTRL_JOY_5 },
	{ KEY_6,	CTRL_JOY_6 },
	{ KEY_7,	CTRL_JOY_7 },
	{ KEY_8,	CTRL_JOY_8 },
};

const u8 g_BonusData[8+1] = { 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0 };
const u8 g_WallData[4+1] = { 0xE1, 0xE2, 0xE8, 0xEF, 0 };

const c8 g_DescBattleRoyal[] = "        BATTLE ROYAL: THE LAST SURVIVOR WINS A ROUND. THE MATCH ENDS WHEN THE CHOSEN NUMBER OF ROUNDS IS REACHED. FIELD COLAPSE AFTER TIMER END.";
const c8 g_DescDeathMatch[]  = "        DEATH MATCH: THE ONE WHO ELIMINATES AN OPPONENT WINS A POINT. THE FIRST TO REACH THE CHOSEN NUMBER OF KILLS (ROUNDS) WINS THE MATCH. FIELD COLAPSE AFTER TIMER END.";
const c8 g_DescSizeMatter[]  = "        SIZE MATTER: AT THE END OF THE CHOSEN TIME, THE ONE WHO HAS REACHED THE LARGEST SIZE WINS THE MATCH.";
const c8 g_DescGreediest[]   = "        GREEDIEST: THE ONE WHO GETS THE MOST BONUSES AT THE END OF THE SET TIME WINS THE GAME.";

const ModeInfo g_ModeInfo[] =
{
	{ "BATTLE ROYAL", g_DescBattleRoyal, sizeof(g_DescBattleRoyal) },
	{ "DEATH MATCH",  g_DescDeathMatch,  sizeof(g_DescDeathMatch) },
	{ "SIZE MATTER",  g_DescSizeMatter,  sizeof(g_DescSizeMatter) },
	{ "GREEDIEST",    g_DescGreediest,   sizeof(g_DescGreediest) },
};

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
u8			g_Scroll;
bool		g_OptMusic = TRUE;
bool		g_OptSFX = TRUE;

// Gameplay
u8			g_GameMode = MODE_BATTLEROYAL;
u8			g_GameCount = 3;
Player		g_Players[PLAYER_MAX];	// Players information
u8			g_PlayerMax;
Vector		g_BonusPos;				// Bonus information
u8			g_BonusTile;
u8			g_BonusOpt = 4;
c8			g_StrBuffer[32];		// String temporary buffer
u8			g_ScreenBuffer[32*24];
u8			g_CurrentPlayer;
u8			g_WallNum = 0;
u8			g_WallOpt = 0;
u8			g_TimeMax = 5;
u8			g_Counter;

// Input
u8			g_JoyInfo;
u8			g_JoyNum;
u8			g_PrevRow3;
u8			g_PrevRow8;
u8			g_Input[CTRL_PLY_NUM];
u8			g_SlotIdx;
bool		g_SelectEdit;
u8			g_CtrlReg[CTRL_MAX];

//=============================================================================
// FUNCTIONS
//=============================================================================

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
void Bios_Exit(u8 ret)
{
	ret;	// A
#if (TARGET_TYPE == TYPE_DOS)

	__asm
	#if BIOS_USE_VDP
		push	af
		// Set Screen mode to 2...
		ld		a, #2
		ld		ix, #R_CHGMOD
		ld		iy, (M_EXPTBL-1)
		call	R_CALSLT
		// ... to be able to call TOTEXT routine
		ld		ix, #R_TOTEXT
		ld		iy, (M_EXPTBL-1)
		call	R_CALSLT
		pop		af
	#endif
		ld		b, a
		ld		c, #DOS_FUNC_TERM
		call	BDOS
		ld		c, #DOS_FUNC_TERM0
		jp		BDOS
	__endasm;

#elif (TARGET_TYPE == TYPE_BIN)
	
	__asm
	#if BIOS_USE_VDP
		// Set Screen mode to 2...
		ld		a, #2
		call	R_CHGMOD
		// ... to be able to call TOTEXT routine
		call	R_TOTEXT
	#endif
		// 
		ld		ix, #0x409B // address of "warm boot" BASIC interpreter
		// this routine is called to reset the stack if basic is externally stopped and then restarted.
		call	R_CALBAS
	__endasm;

#else // if (TARGET_TYPE == TYPE_ROM)

	__asm
		call	0x0000				// Soft reset
	__endasm;

#endif
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
// 
inline void ClearLevel()
{
	Mem_Set(TILE_EMPTY, g_ScreenBuffer, 32*24);
}

//-----------------------------------------------------------------------------
// 
inline void DrawLevel()
{
	VDP_WriteVRAM(g_ScreenBuffer, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32*24);
}

//-----------------------------------------------------------------------------
// 
inline u8 GetTile(u8 x, u8 y)
{
	return g_ScreenBuffer[x + (y * 32)];
}

//-----------------------------------------------------------------------------
// 
inline void DrawTile(u8 x, u8 y, c8 chr)
{
	g_ScreenBuffer[x + (y * 32)] = chr;
}

//-----------------------------------------------------------------------------
// 
void DrawTileX(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		DrawTile(x++, y, chr);
}

//-----------------------------------------------------------------------------
// 
void DrawTileY(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		DrawTile(x, y++, chr);
}

//-----------------------------------------------------------------------------
//
inline void SetScore(Player* ply)
{
	Print_DrawIntAt(ply->ID * 4 + 1, 0, ply->Score);
}

//-----------------------------------------------------------------------------
//
void DrawCounter(u8 x, u8 y, u8 step)
{
	u8 x0 = x;
	u16 bit = 0b0100000000000000;
	for(u8 i = 15; i > 0; --i)
	{
		if(g_TimerLayout[step] & bit)
			PrintChr(x, y, 0xE0);
		else
			PrintChr(x, y, TILE_EMPTY);
		x++;
		if(x > x0 + 2)
		{
			x -= 3;
			y++;
		}
		bit >>= 1;
	}
}

//-----------------------------------------------------------------------------
//
void MoveCursor(i8 idx)
{
	idx %= numberof(g_SelectSlot);
	g_SlotIdx = idx;

	VDP_SetSpriteSM1(0, 0, 0, 16, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(1, 0, 0, 17, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(2, 0, 0, 18, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1(3, 0, 0, 19, COLOR_DARK_BLUE);
}

//-----------------------------------------------------------------------------
//
void EditPlayer(u8 id, bool bEdit)
{
	u8 x = g_CharaInfo[id].FrameX;
	u8 y = g_CharaInfo[id].FrameY;
	u8 ctrl = g_Players[id].Controller;
	VDP_WriteLayout_GM2(bEdit ? g_Devices[ctrl].Edit : g_Devices[ctrl].Default, x, y + 6, 7, 3);
	g_SelectEdit = bEdit;

	if(bEdit)
	{
		VDP_SetSpritePosition(0, g_SelectSlot[g_SlotIdx].UL.X, g_SelectSlot[g_SlotIdx].UL.Y);
		VDP_SetSpritePosition(1, g_SelectSlot[g_SlotIdx].DR.X, g_SelectSlot[g_SlotIdx].UL.Y);
		VDP_SetSpritePosition(2, g_SelectSlot[g_SlotIdx].UL.X, g_SelectSlot[g_SlotIdx].DR.Y + 16);
		VDP_SetSpritePosition(3, g_SelectSlot[g_SlotIdx].DR.X, g_SelectSlot[g_SlotIdx].DR.Y + 16);
	}
}

//-----------------------------------------------------------------------------
//
u8 GetHumanCount()
{
	u8 count = 0;
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		if((g_Players[i].Controller < CTRL_PLY_NUM) && (g_Players[i].State != STATE_NONE))
			count++;

	return count;
}

//-----------------------------------------------------------------------------
//
void CheckBattleRoyal()
{
	// Check is there is a last standing
	Player* lastPly = NULL;
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		if((g_Players[i].Controller != CTRL_NONE) && (g_Players[i].State != STATE_NONE))
		{
			if(lastPly)
				return;
			lastPly = &g_Players[i];
		}
	}

	// Score a point
	lastPly->Score++;
	SetScore(lastPly);

	// Check victory condition
	/* ... */

	// Clean field and start a new round
	ClearPlayer(lastPly);
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		SpawnPlayer(&g_Players[i]);
}

//-----------------------------------------------------------------------------
// 
void SpawnBonus()
{
	u8 rnd = Math_GetRandom8();
	u8 x = 8 + rnd % 16;
	u8 y = 8 + (rnd >> 4) % 8;
	while(VDP_Peek_GM2(x, y) != TILE_EMPTY)
		x++;
	g_BonusPos.X = x;
	g_BonusPos.Y = y;
	g_BonusTile = g_BonusData[g_BonusOpt];
	if(g_BonusTile == 0)
		g_BonusTile = g_BonusData[Math_GetRandom8() & 0x07];
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
		if(VDP_Peek_GM2(x, y) < 0xF0)
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

	if((ply->Controller == CTRL_AI_EASY) && (rnd & 0x03 != 0)) // 3 in 4 chances to do nothing
		return;
	else if((ply->Controller == CTRL_AI_MED) && (rnd & 0x03 == 0)) // 1 in 4 chances to do nothing
		return;

	i8 weight[ACTION_MAX] = { 0, 0, 0 };

	u8 x = ply->PosX;
	u8 y = ply->PosY;

	// Front
	u8 dist = CheckDir(x, y, ply->Dir, 3);
	if(dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_LEFT] += w;
		weight[ACTION_RIGHT] += w;
	}

	// Check right
	dist = CheckDir(x, y, (ply->Dir + 1) & 0x3 /*% DIR_MAX*/, 3);
	if(dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_NONE] += w;
		weight[ACTION_LEFT] += w;
	}

	// Check left
	dist = CheckDir(x, y, (ply->Dir + DIR_MAX - 1) & 0x3 /*% DIR_MAX*/, 3);
	if(dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_NONE] += w;
		weight[ACTION_RIGHT] += w;
	}

	// Seek Bonus
	switch(ply->Dir)
	{
	case DIR_UP:
		if(g_BonusPos.X > ply->PosX)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if(g_BonusPos.X < ply->PosX)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_DOWN:
		if(g_BonusPos.X < ply->PosX)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if(g_BonusPos.X > ply->PosX)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_RIGHT:
		if(g_BonusPos.Y > ply->PosY)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if(g_BonusPos.Y < ply->PosY)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;

	case DIR_LEFT:
		if(g_BonusPos.Y < ply->PosY)
			weight[ACTION_RIGHT] += AI_WEIGHT_BONUS;
		else if(g_BonusPos.Y > ply->PosY)
			weight[ACTION_LEFT] += AI_WEIGHT_BONUS;
		break;
	}

	// Randomize
	weight[ACTION_NONE] += rnd & 0x07;

	// Select the choice with the biggest weight
	u8 choice = ACTION_NONE;
	if(weight[ACTION_NONE] < weight[ACTION_RIGHT])
		choice = ACTION_RIGHT;
	else if((weight[ACTION_NONE] == weight[ACTION_RIGHT]) && (rnd & 0x01))
		choice = ACTION_RIGHT;
	if(weight[choice] < weight[ACTION_LEFT])
		choice = ACTION_LEFT;
	else if((weight[choice] == weight[ACTION_LEFT]) && (rnd & 0x01))
		choice = ACTION_LEFT;

	// Apply choice
	if(choice == ACTION_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if(choice == ACTION_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
}

//-----------------------------------------------------------------------------
//
void UpdatePlayerInput(Player* ply)
{
	if(g_Input[ply->Controller] == ACTION_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if(g_Input[ply->Controller] == ACTION_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
	g_Input[ply->Controller] = ACTION_NONE;
}

//-----------------------------------------------------------------------------
// 
void SetPlayerController(Player* ply, u8 ctrl)
{
	// Register device
	if(ply->Controller < CTRL_PLY_NUM) // Free previous
		g_CtrlReg[ply->Controller] = CTRL_FREE;
	if(ctrl < CTRL_PLY_NUM) // Reserve next
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
		if(ctrl > 0)
			ctrl--;
		else
			ctrl = CTRL_MAX - 1;
		if((ctrl <= CTRL_JOY_8) && (ctrl >= g_JoyNum))
			continue;
		if(g_CtrlReg[ctrl] == CTRL_FREE)
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
		if(ctrl < CTRL_MAX - 1)
			ctrl++;
		else
			ctrl = 0;
		if((ctrl <= CTRL_JOY_8) && (ctrl >= g_JoyNum))
			continue;
		if(g_CtrlReg[ctrl] == CTRL_FREE)
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
	ply->State      = STATE_NONE;

	if(id < g_JoyNum)
		SetPlayerController(ply, id);
	else if(id < g_PlayerMax)
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
	if(ply->Controller == CTRL_NONE)
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

	SetScore(ply);
}

//-----------------------------------------------------------------------------
//
void DrawPlayer(Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;
	u8 baseTile = g_CharaInfo[ply->ID].TileBase;
	bool bGrow = FALSE;
	bool bReduce = FALSE;

	// Grow crawler length
	if((ply->Length < ply->Expect) && (ply->Length < LENGTH_MAX))
	{
		ply->Length++;
		if((g_GameMode == MODE_SIZEMATTER) && (ply->Length > ply->Score))
		{
			ply->Score = ply->Length;
			SetScore(ply);
		}
		bGrow = TRUE;
	}
	// Reduce crawler length
	else if((ply->Length > ply->Expect) && (ply->Length > LENGTH_MIN))
	{
		ply->Length--;
		bReduce = TRUE;
	}

	// Draw crawler
	for(u8 i = 0; i < ply->Length + 1; ++i)
	{
		// Head
		if(i == 0)
		{
			u8 tile = 0x40 + ply->Path[idx];
			VDP_Poke_GM2(x, y, tile + baseTile);

			// Sprite
			const Character* chr = &g_CharaInfo[ply->ID];
			if(chr->Sprite != 0xFF)
			{
				if(chr->Sprite & 1) // Snake
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
		else if(i == 1)
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			const Shapes* vec = &g_Body[ply->Path[prev] + (ply->Path[idx] << 2)];
			u8 tile;
			if(idx & 1)
				tile = vec->A;
			else
				tile = vec->B;
			VDP_Poke_GM2(x, y, tile + baseTile);
		}
		// Tail
		else if(i == ply->Length - 1)
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			u8 tile = 0x4C + ply->Path[prev] * 2;
			if(ply->Anim & 1)
				tile++;
			VDP_Poke_GM2(x, y, tile + baseTile);
		}
		// Clear
		else if((!bGrow) && (i == ply->Length))
		{
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
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;

	// Clear tiles
	for(u8 i = 0; i < ply->Length; ++i)
	{
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
	if(chr->Sprite != 0xFF)
		VDP_HideSprite(chr->Sprite);

	ply->State = STATE_NONE;
}

//-----------------------------------------------------------------------------
// Program entry point
void UpdatePlayer(Player* ply)
{
	if(ply->Controller == CTRL_NONE)
		return;

	switch(ply->State)
	{
	case STATE_NONE:
		return;

	case STATE_INIT:
		ply->Timer = COOLDOWN_WAIT;
		ply->State = STATE_COOLDOWN;

	case STATE_COOLDOWN:
		if(VDP_Peek_GM2(ply->PosX, ply->PosY) >= TILE_EMPTY)
		{
			u8 tile = (u8)(TILE_PREHOLE + 1);
			if(ply->Timer <= (COOLDOWN_WAIT / 4))
				tile++;
			VDP_Poke_GM2(ply->PosX, ply->PosY, tile);
		}
		ply->Timer--;
		if(ply->Timer)
			return;
		ply->State = STATE_WAITING;

	case STATE_WAITING:
		if(VDP_Peek_GM2(ply->PosX, ply->PosY) < TILE_EMPTY)
			return;
		ply->Timer = SPAWN_WAIT;
		ply->State = STATE_SPAWNING;

	case STATE_SPAWNING:
	{
		// Get input action
		ply->Action(ply);

		// Diplay hole
		u8 tile = TILE_HOLE;
		if((ply->Timer < (3 * SPAWN_WAIT / 4)) && (ply->Timer & 1))
			tile += 1 + ply->Dir;
		VDP_Poke_GM2(ply->PosX, ply->PosY, tile);

		// Check timer
		ply->Timer--;
		if(ply->Timer)
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
		if(cell < 0xF0) // Obstacle
		{
			ClearPlayer(ply);
			switch(g_GameMode)
			{
			case MODE_BATTLEROYAL:
				CheckBattleRoyal();
				g_DoSynch = (GetHumanCount() > 0);
				return;
			case MODE_DEATHMATCH:
			case MODE_SIZEMATTER:
			case MODE_GREEDIEST:
				SpawnPlayer(ply);
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
			case TILE_BONUS+7:
				ply->Expect += BONUS_GROWTH;
				SpawnBonus();
				if(g_GameMode == MODE_GREEDIEST)
				{
					ply->Score++;
					SetScore(ply);
				}
			default:
				ply->PosX = x;
				ply->PosY = y;
				ply->Idx--;
				ply->Idx %= LENGTH_MAX;
				ply->Path[ply->Idx] = ply->Dir;
				DrawPlayer(ply);
				break;
			}
		}
		return;
	}
	}
}

//-----------------------------------------------------------------------------
// VBlank interrupt
void VDP_InterruptHandler()
{
	g_VBlank = TRUE;

	g_6thFrameCount++;
	if(g_6thFrameCount == 5)
		g_6thFrameCount = 0;

	if((g_Freq == FREQ_50HZ) || (g_6thFrameCount))
		AKG_Decode();
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
wait_loop:
	while(!g_VBlank) {} // Wait for VDP interruption
	g_VBlank = FALSE;

	// Skip the 6th frame on 60 Hz VDP
	if((g_Freq == FREQ_60HZ) && (!g_6thFrameCount))
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
	if(op == MENU_ACTION_SET)
		FSM_SetState(&State_Select);
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
		if(g_GameMode < MODE_MAX-1)
			g_GameMode++;
		else
			g_GameMode = 0;
		g_Scroll = 0;
		break;

	case MENU_ACTION_DEC:
		if(g_GameMode > 0)
			g_GameMode--;
		else
			g_GameMode = MODE_MAX-1;
		g_Scroll = 0;
		break;

	case MENU_ACTION_GET:
		return g_ModeInfo[g_GameMode].Name;
	};
	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Info(u8 op, i8 value)
{
	op;
	value;
	if(g_Frame & 0x7)
		return NULL;

	if(g_MenuItem != 1)
		return "";

	const ModeInfo* info = &g_ModeInfo[g_GameMode];
	u8 size = info->Size - 1;

	u8 j = g_Scroll;
	while(j >= size)
		j -= size;

	for(u8 i = 0; i < 20; ++i)
	{
		g_StrBuffer[i] = info->Desc[j];
		if(++j >= size)
			j -= size;
	}
	g_StrBuffer[20] = 0;

	g_Scroll++;

	return g_StrBuffer;
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

	if(g_FreqOpt == FREQ_60HZ) 
	{
		g_Freq = FREQ_60HZ;
		return "60 HZ";
	}
	else if(g_FreqOpt == FREQ_50HZ)
	{
		g_Freq = FREQ_50HZ;
		return "50 HZ";
	}
	else
	{
		g_Freq = g_FreqDetected;
		if(g_Freq == FREQ_50HZ)
			return "AUTO (50 HZ)";
		else
			return "AUTO (60 HZ)";
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Palette(u8 op, i8 value)
{
	value;
	if(g_VersionVDP == VDP_VERSION_TMS9918A)
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
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Music(u8 op, i8 value)
{
	value;
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
	case MENU_ACTION_DEC:
		TOGGLE(g_OptMusic);
		if(g_OptMusic)
			AKG_Init((const void*)0xD000, 0);
		else if(!g_OptMusic)
			AKG_Stop();
		break;
	}
	return g_OptMusic ? "#" : "&";
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
		if(g_BonusOpt >= sizeof(g_BonusData))
			g_BonusOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_BonusOpt--;
		if(g_BonusOpt == 255)
			g_BonusOpt = sizeof(g_BonusData) - 1;
		break;
	}

	if(g_BonusData[g_BonusOpt] == 0)
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
		if(g_WallOpt >= sizeof(g_WallData))
			g_WallOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_WallOpt--;
		if(g_WallOpt == 255)
			g_WallOpt = sizeof(g_WallData) - 1;
		break;
	}

	if(g_WallData[g_WallOpt] == 0)
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
	if(value == 0) // port 1
		tap = (g_JoyInfo >> 4) & 0x03;
	else if(value == 1)
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
	switch(g_MSXVER)
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
const c8* MenuAction_Exit(u8 op, i8 value)
{
	value;
	if(op == MENU_ACTION_SET)
		Bios_Exit(0);
	return NULL;
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
	if(Keyboard_IsKeyPressed(KEY_1))
		g_VersionVDP = VDP_VERSION_TMS9918A;
	else if(Keyboard_IsKeyPressed(KEY_2))
		g_VersionVDP = VDP_VERSION_V9938;
	else
		g_VersionVDP = VDP_GetVersion();

	// Initialize palette
	if(g_VersionVDP > VDP_VERSION_TMS9918A)
	{
		VDP_SetPalette((u8*)g_MSX2Palette);
		g_PalOpt = PAL_CUSTOM;
	}

	// Initialize frequency
	if(g_BASRVN[0] & 0x80)
		g_FreqDetected = FREQ_50HZ;
	else
		g_FreqDetected = FREQ_60HZ;
	g_Freq = g_FreqDetected;

	// Set VBlank hook
	VDP_EnableVBlank(TRUE);
	Bios_SetHookDirectCallback(H_TIMI, VDP_InterruptHandler);

	// Initialize Joystick and/or Ninja Tap
	NTap_Check();
	g_JoyInfo = NTap_GetInfo();
	g_JoyNum = NTap_GetPortNum();
	g_PlayerMax = MIN(g_JoyNum + 2, 8);

	// Initialize free device
	for(u8 i = 0; i < CTRL_MAX; ++i)
	{
		g_CtrlReg[i] = ((i >= g_JoyNum) && (i <= CTRL_JOY_8)) ? CTRL_UNAVAILABLE : CTRL_FREE;
	}

	// Initialize players
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		InitPlayer(&g_Players[i], i);

	// Load music
	Pletter_UnpackToRAM(g_MusicMain, (void*)0xD000);
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
	VDP_LoadSpritePattern(g_DataLogoSprt, 4, sizeof(g_DataLogoSprt) / 8);
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
	VDP_LoadPattern_GM2(g_DataLogoTile_Patterns, sizeof(g_DataLogoTile_Patterns) / 8, 0);
	VDP_LoadColor_GM2(g_DataLogoTile_Colors, sizeof(g_DataLogoTile_Colors) / 8, 0);
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

	if((g_Frame & 0x03) != 0)
		return;

	g_Counter++;
	if((g_Counter >= LOGO_START) && (g_Counter < LOGO_START + LOGO_OFFSET))
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

	if((g_Counter == LOGO_END) || Keyboard_IsKeyPressed(KEY_SPACE))
		FSM_SetState(&State_Title);
}

//.............................................................................
// TITLE MENU STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Title_Begin()
{
	// Initialize music
	if(g_OptMusic)
		AKG_Init((const void*)0xD000, 0);

	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_SetColor(COLOR_LIGHT_YELLOW);
	VDP_ClearVRAM();

	// Initialize sprites data
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_8);
	VDP_LoadSpritePattern(g_DataSprites, 0, sizeof(g_DataSprites) / 8);
	VDP_LoadSpritePattern(g_DataEyes, 32, sizeof(g_DataEyes) / 8);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);
	VDP_WriteLayout_GM2(g_TitleTile, 4, 3, 24, 5);

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

	VDP_DisableSpritesFrom(0);

	VDP_EnableDisplay(TRUE);
}


//-----------------------------------------------------------------------------
//
void State_Title_Update()
{
	// Wait V-Synch
	WaitVBlank();

	Print_DrawTextAt(11, 15, (g_Frame & 0x10) ? "PRESS SPACE" : "           ");

	if(Keyboard_IsKeyPressed(KEY_SPACE))
		FSM_SetState(&State_Menu);
}

//.............................................................................
// MAIN MENU STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Menu_Begin()
{
	State_Title_Begin();

	// Initialize menu
	Menu_Initialize(g_Menus);
	Menu_DrawPage(MENU_MAIN);
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
// SELECT STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Select_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);

	//........................................
	// Load tiles

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataSelect_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataSelect_Colors, 0);
	// Portaits tiles
	u16 dst = g_ScreenPatternLow + (128 * 8);
	Pletter_UnpackToVRAM(g_DataFace1_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace1_Patterns, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(g_DataFace2_Patterns, dst);
	// Portaits colors
	dst = g_ScreenColorLow + (128 * 8);
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
	VDP_WriteLayout_GM2(SELECT_START, 2, 2, 7, 2);
	VDP_WriteLayout_GM2(SELECT_EXIT, 24, 2, 6, 2);
	// Frames and devices
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		u8 x = g_CharaInfo[i].FrameX;
		u8 y = g_CharaInfo[i].FrameY;
		VDP_WriteLayout_GM2(SELECT_FRAME, x, y, 7, 6);
		VDP_WriteLayout_GM2(g_CharaInfo[i].Face, x + 1, y + 1, 5, 5);
		u8 ctrl = g_Players[i].Controller;
		VDP_WriteLayout_GM2(g_Devices[ctrl].Default, x, y + 6, 7, 3);
	}

	//........................................
	// Eyes
	VDP_SetSpriteSM1( 4,  50,  67, 32, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1( 5, 102,  64, 33, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1( 6, 158,  63, 34, COLOR_BLACK);
	VDP_SetSpriteSM1( 7, 214,  65, 35, COLOR_BLACK);
	VDP_SetSpriteSM1( 8, 162, 139, 32, COLOR_DARK_BLUE);
	VDP_SetSpriteSM1( 9, 214, 136, 33, COLOR_BLACK);
	VDP_SetSpriteSM1(10,  46, 135, 34, COLOR_BLACK);
	VDP_SetSpriteSM1(11, 102, 137, 35, COLOR_DARK_BLUE);
	VDP_DisableSpritesFrom(12);

	g_SelectEdit = FALSE;
	g_PrevRow8 = 0;
	MoveCursor(8);

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_Select_Update()
{
	// Wait V-Synch
	WaitVBlank();

	u8 row8 = Keyboard_Read(8);

	if(g_SelectEdit) // Handle character edit
	{
		Player* ply = &g_Players[g_SlotIdx];
		if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))
		{
			SetPrevPlayerController(ply);
			EditPlayer(g_SlotIdx, TRUE);
		}
		else if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))
		{
			SetNextPlayerController(ply);
			EditPlayer(g_SlotIdx, TRUE);
		}

		if(IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(g_PrevRow8, KEY_SPACE))
			EditPlayer(g_SlotIdx, FALSE);
	}
	else // Handle menu update
	{
		// Move cursor
		i8 newSlot = -1;
		if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))
			newSlot = g_SelectSlot[g_SlotIdx].Left;
		else if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))
			newSlot = g_SelectSlot[g_SlotIdx].Right;
		if(IS_KEY_PRESSED(row8, KEY_UP) && !IS_KEY_PRESSED(g_PrevRow8, KEY_UP))
			newSlot = g_SelectSlot[g_SlotIdx].Up;
		else if(IS_KEY_PRESSED(row8, KEY_DOWN) && !IS_KEY_PRESSED(g_PrevRow8, KEY_DOWN))
			newSlot = g_SelectSlot[g_SlotIdx].Down;
		if(newSlot != -1)
			MoveCursor(newSlot);

		// Animate cursor
		u8 offset = g_CursorAnim[(g_Frame >> 2) % 8];
		VDP_SetSpritePosition(0, g_SelectSlot[g_SlotIdx].UL.X - offset, g_SelectSlot[g_SlotIdx].UL.Y - offset);
		VDP_SetSpritePosition(1, g_SelectSlot[g_SlotIdx].DR.X + offset, g_SelectSlot[g_SlotIdx].UL.Y - offset);
		VDP_SetSpritePosition(2, g_SelectSlot[g_SlotIdx].UL.X - offset, g_SelectSlot[g_SlotIdx].DR.Y + offset);
		VDP_SetSpritePosition(3, g_SelectSlot[g_SlotIdx].DR.X + offset, g_SelectSlot[g_SlotIdx].DR.Y + offset);

		// Handle validation
		if(IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(g_PrevRow8, KEY_SPACE))
		{
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
				FSM_SetState(&State_Start);
				break;
			case 9:
				FSM_SetState(&State_Menu);
				break;
			};
		}

		if(Keyboard_IsKeyPressed(KEY_RET))
			FSM_SetState(&State_Start);
		if(Keyboard_IsKeyPressed(KEY_ESC))
			FSM_SetState(&State_Menu);

		// Handle special keys
		if(g_SlotIdx < 8)
		{
			Player* ply = &g_Players[g_SlotIdx];
			for(u8 i = 0; i < numberof(g_CtrlBind); ++i)
			{
				if(g_CtrlReg[g_CtrlBind[i].Ctrl] == CTRL_FREE)
				{
					if(Keyboard_IsKeyPressed(g_CtrlBind[i].Key))
					{
						SetPlayerController(ply, g_CtrlBind[i].Ctrl);
						EditPlayer(g_SlotIdx, FALSE);
						break;
					}
				}
			}
		}
	}

	g_PrevRow8 = row8;
}

//.............................................................................
// START STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Start_Begin()
{
	VDP_EnableDisplay(FALSE);

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Initialize sprites data
	VDP_DisableSpritesFrom(0);

	// Draw game field
	ClearLevel();
	DrawTile(0,  1, 0xE9);
	DrawTile(31, 1, 0xEA);
	DrawTileX(1,  1, 0xE8, 30);
	DrawTileY(0,  2, 0xEB, 21);
	DrawTileY(31, 2, 0xEC, 21);
	DrawTile(0, 23, 0xED);
	DrawTile(31, 23, 0xEE);
	DrawTileX(1, 23, 0xE8, 30);

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
	u8 wallTile = g_WallData[g_WallOpt];
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
			if(GetTile(x, y) != TILE_EMPTY)
				bLoop = TRUE;
			// No obstacle in hole lines
			for(u8 j = 0; j < PLAYER_MAX; ++j)
			{
				if(x == g_Starts[j].X)
				{
					bLoop = TRUE;
					break;
				}
				if(y == g_Starts[j].Y)
				{
					bLoop = TRUE;
					break;
				}
			}
		}
		u8 wall = wallTile;
		if(wall == 0)
			wall = g_WallData[Math_GetRandom8() & 0x03];
		DrawTile(x, y, wall);
	}

	// Copy screen buffer to VRAM
	DrawLevel();

	// Spawn players
	for(u8 i = 0; i < PLAYER_MAX; ++i)
		ResetPlayer(&g_Players[i]);

	g_CurrentPlayer = 0;
	g_PrevRow3 = 0;
	g_PrevRow8 = 0;

	g_DoSynch = (GetHumanCount() > 0);

	g_Counter = 0;

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_Start_Update()
{
	// Wait V-Synch
	WaitVBlank();

	// Skip start sequence
	if((g_Frame & 0x0F) != 0)
		return;

	// Display character and control
	g_Counter++;

	if(g_Counter & 1)
	{
		for(u8 i = 0; i < PLAYER_MAX; ++i)
		{
			const Player* ply = &g_Players[i];
			if(ply->Controller != CTRL_NONE)
			{
				u8 x = ply->PosX;
				u8 y = ply->PosY;
				PrintChr(x, y, TILE_PREHOLE);
				PrintChrX(++x, y, TILE_EMPTY, 2);
			}
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
				num = 17 + ply->Controller;
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
				tile = TILE_AI;
				break;
			case CTRL_NONE:
				continue;
			}

			u8 x = ply->PosX;
			u8 y = ply->PosY;
			PrintChr(x, y, 0x42 + g_CharaInfo[i].TileBase);
			PrintChr(++x, y, tile);
			if(num != 0xFF)
				PrintChr(++x, y, num);
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
		FSM_SetState(&State_Game);
		return;
	}

	// Check input
	if(Keyboard_IsKeyPressed(KEY_SPACE))
		FSM_SetState(&State_Game);
	if(Keyboard_IsKeyPressed(KEY_ESC))
		FSM_SetState(&State_Menu);
}

//.............................................................................
// GAME STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Game_Begin()
{
	// Initialize sprites data
	VDP_HideSprite(0);
	VDP_HideSprite(1);
	VDP_HideSprite(2);
	VDP_HideSprite(3);
	VDP_DisableSpritesFrom(4);

	// Copy screen buffer to VRAM
	DrawLevel();

	// Draw pre-hole
	for(u8 i = 0; i < PLAYER_MAX; ++i)
	{
		const Start* start = &g_Starts[i];
		PrintChr(start->X, start->Y, TILE_PREHOLE);
	}

	// Initialize Bonus
	SpawnBonus();

	// AKG_Stop();
}

//-----------------------------------------------------------------------------
//
void State_Game_Update()
{
	// Wait V-Synch
	if(g_DoSynch)
		WaitVBlank();

	// Update one of the players
	Player* ply = &g_Players[g_CurrentPlayer];
	UpdatePlayer(ply);
	g_CurrentPlayer++;
	g_CurrentPlayer %= PLAYER_MAX;

	VDP_Poke_GM2(g_BonusPos.X, g_BonusPos.Y, g_BonusTile);

	// Update keyboard entries
	if(g_Input[CTRL_KEY_1] == ACTION_NONE)
	{
		u8 row8 = Keyboard_Read(8);
		if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))
			g_Input[CTRL_KEY_1] = ACTION_LEFT;
		else if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))
			g_Input[CTRL_KEY_1] = ACTION_RIGHT;
		g_PrevRow8 = row8;
	}
	if(g_Input[CTRL_KEY_2] == ACTION_NONE)
	{
		u8 row3 = Keyboard_Read(3);
		if(IS_KEY_PRESSED(row3, KEY_D) && !IS_KEY_PRESSED(g_PrevRow3, KEY_D))
			g_Input[CTRL_KEY_2] = ACTION_LEFT;
		else if(IS_KEY_PRESSED(row3, KEY_G) && !IS_KEY_PRESSED(g_PrevRow3, KEY_G))
			g_Input[CTRL_KEY_2] = ACTION_RIGHT;
		g_PrevRow3 = row3;
	}
	// Update joysticks
	NTap_Update();
	for(u8 i = 0; i < g_JoyNum; ++i)
	{
		if(g_Input[i] == ACTION_NONE)
		{
			if(NTap_IsPushed(i, NTAP_LEFT))
				g_Input[i] = ACTION_LEFT;
			else if(NTap_IsPushed(i, NTAP_RIGHT))
				g_Input[i] = ACTION_RIGHT;
		}
	}

	if(Keyboard_IsKeyPressed(KEY_ESC))
		FSM_SetState(&State_Menu);
}

//.............................................................................
// VICTORY STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Victory_Begin()
{
	
}

//-----------------------------------------------------------------------------
//
void State_Victory_Update()
{
	
}

//=============================================================================
// MAIN
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	// Start the Final State Machine
	FSM_SetState(&State_Init);
	while(1)
		FSM_Update();

	Bios_Exit(0);
}