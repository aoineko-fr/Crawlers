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
#include "device/pac.h"
#include "arkos/akg_player.h"
#include "compress/pletter.h"
#include "dos.h"
#include "crypt.h"

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

void State_CtrlTest_Begin();
void State_CtrlTest_Update();

void State_ScoreBoard_Begin();
void State_ScoreBoard_Update();

void MenuOpen_Multi();
void MenuOpen_Solo();
void MenuOpen_Control();
void MenuOpen_Credit();
void MenuOpen_Save();

const c8* MenuAction_Start(u8 op, i8 value);
const c8* MenuAction_Mode(u8 op, i8 value);
const c8* MenuAction_Speed(u8 op, i8 value);
const c8* MenuAction_Info(u8 op, i8 value);
const c8* MenuAction_Credits(u8 op, i8 value);
const c8* MenuAction_Freq(u8 op, i8 value);
const c8* MenuAction_Palette(u8 op, i8 value);
const c8* MenuAction_Music(u8 op, i8 value);
const c8* MenuAction_SFX(u8 op, i8 value);
const c8* MenuAction_PAC(u8 op, i8 value);
const c8* MenuAction_Bonus(u8 op, i8 value);
const c8* MenuAction_Wall(u8 op, i8 value);
const c8* MenuAction_Port(u8 op, i8 value);
const c8* MenuAction_MSX(u8 op, i8 value);
const c8* MenuAction_VDP(u8 op, i8 value);
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

#if (TARGET_TYPE == TYPE_DOS) // Defined in system.h for 48K ROM
u8 g_VersionROM = 0;
u8 g_VersionMSX = 0;
#endif

extern MenuItemMinMax g_MenuLevelMinMax;

//=============================================================================
// READ-ONLY DATA
//=============================================================================

#if (TARGET_TYPE == TYPE_DOS)
	#include "crawlers_p0.c"
#endif

const MenuItemMinMax g_MenuRoundsMinMax = { 1, 20, 1 };
const MenuItemMinMax g_MenuTreesMinMax =  { 0, 100, 10 };
const MenuItemMinMax g_MenuPACMinMax =  { 1, 8, 1 };

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
	{ "LEVEL",               MENU_ITEM_INT, &g_Option.TrainLevel, (i16)&g_MenuLevelMinMax },
	{ "SPEED",               MENU_ITEM_ACTION, MenuAction_Speed, 0 },
	{ "SCORES",              MENU_ITEM_ACTION, MenuAction_Start, START_SCORE_BOARD },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

// MENU_MULTI - Multiplayer battle menu
const MenuItem g_MenuMulti[] =
{
	{ "START",               MENU_ITEM_ACTION, MenuAction_Start, START_BATTLE },
	{ "MODE",                MENU_ITEM_ACTION, MenuAction_Mode, 0 },
	{ "COUNT",               MENU_ITEM_INT, &g_Option.GameCount, (i16)&g_MenuRoundsMinMax },
	{ "WALLS",               MENU_ITEM_INT, &g_Option.WallNum, (i16)&g_MenuTreesMinMax },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ NULL,                  MENU_ITEM_UPDATE, MenuAction_Info, -1 },
};

// MENU_OPTION - Options menu
const MenuItem g_MenuOption[] =
{
	{ "GRAPHICS",            MENU_ITEM_GOTO, NULL, MENU_GRAPH },
	{ "CONTROL",             MENU_ITEM_GOTO, NULL, MENU_CONTROL },
	{ "AUDIO",               MENU_ITEM_GOTO, NULL, MENU_AUDIO },
	{ "PAC",                 MENU_ITEM_GOTO, NULL, MENU_SAVE },
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
	{ "TEST",                MENU_ITEM_ACTION, MenuAction_Start, START_CTRL_TEST },
	{ "TURN",                MENU_ITEM_ACTION, MenuAction_Turn, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_OPTION },
};

// MENU_SAVE - Save page
const MenuItem g_MenuSave[] =
{
	{ "SLOT",                MENU_ITEM_ACTION|MENU_ITEM_DISABLE, MenuAction_PAC, 0 },
	{ "PAGE",                MENU_ITEM_INT, &g_PACPage, (i16)&g_MenuPACMinMax },
	{ "LOAD",                MENU_ITEM_ACTION, MenuAction_PAC, 1 },
	{ "SAVE",                MENU_ITEM_ACTION, MenuAction_PAC, 2 },
	{ "ERASE",               MENU_ITEM_ACTION, MenuAction_PAC, 3 },
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
	{ NULL, g_MenuControl, numberof(g_MenuControl), MenuOpen_Control },	// MENU_CONTROL
	{ NULL, g_MenuAudio,   numberof(g_MenuAudio),   NULL },				// MENU_AUDIO
	{ NULL, g_MenuSave,    numberof(g_MenuSave),    MenuOpen_Save },	// MENU_SAVE
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
const FSM_State State_CtrlTest =		{ 0, State_CtrlTest_Begin,		State_CtrlTest_Update,		NULL };
const FSM_State State_ScoreBoard =		{ 0, State_ScoreBoard_Begin,	State_ScoreBoard_Update,	NULL };

const c8 g_DescGreediest[]   = "            GREEDIEST: EACH TIME YOU EAT A BONUS, YOU EARN A POINT. THE FIRST TO REACH THE CHOSEN NUMBER OF POINTS (COUNT) WINS THE BATTLE.";
const c8 g_DescDeathmatch[]  = "            DEATHMATCH: EACH TIME YOU ELIMINATE AN OPPONENT, YOU EARN A POINT. EACH TIME YOU CRASH INTO AN OBSTACLE, YOU LOSE A POINT. THE FIRST TO REACH THE CHOSEN NUMBER OF POINTS (COUNT) WINS THE BATTLE.";
const c8 g_DescSizeMatters[] = "            SIZE MATTERS: WHEN YOUR LENGTH IS GREATER THAN YOUR SCORE IT BECOMES YOUR NEW SCORE. WHEN OTHERS EAT A BONUS, YOU LOSE A POINT. AFTER THE CHOSEN NUMBER OF MINUTES (COUNT), THE ONE WITH THE MOST POINTS WINS THE BATTLE.";
const c8 g_DescBattleRoyal[] = "            BATTLE ROYAL: THE LAST SURVIVOR OF A ROUND WINS 1 POINT. AFTER ONE MINUTE, IF THE ROUND IS NOT OVER, THE FIELD BEGINS TO COLLAPSE. THE FIRST TO REACH THE CHOSEN NUMBER OF POINTS (COUNT) WINS THE BATTLE.";

const ModeInfo g_ModeInfo[] =
{//   Name            Dec.               Length                     Rnd Time   Length
	{ "GREEDIEST",    g_DescGreediest,   sizeof(g_DescGreediest),   10, FALSE, 5 },
	{ "DEATHMATCH",   g_DescDeathmatch,  sizeof(g_DescDeathmatch),  5,  FALSE, 5 },
	{ "SIZE MATTERS", g_DescSizeMatters, sizeof(g_DescSizeMatters), 2,  TRUE,  5 },
	{ "BATTLE ROYAL", g_DescBattleRoyal, sizeof(g_DescBattleRoyal), 3,  TRUE,  5 },
};

const c8 g_TextCredits[]   = "                CRAWLERS BY PIXEL PHENIX 2023    VERSION " GAME_VERSION "    POWERED BY [\\]^    DESIGN, CODE AND GFX BY GUILLAUME 'AOINEKO' BLANCHARD    MUSIC AND SFX BY THOMAS 'TOTTA' LUNDGREN    GFX ENHANCEMENT BY LUDOVIC 'GFX' AVOT    FONT BY DAMIEN GUARD    THANKS TO ALL MRC, MSX VILLAGE AND [\\]^ DISCORD MEMBERS FOR SUPPORT    MSX STILL ALIVE!!    DEDICATED TO MY WONDERFUL WIFE AND SON \x1F\x1F";

//=============================================================================
// MEMORY DATA
//=============================================================================

// Options
OptionData	g_Option;

// System
bool		g_VBlank = FALSE;		// Vertical-synchronization flag
u8			g_Frame = 0;			// Frame counter
u8			g_6thFrameCount = 0;	// Frame counter
u8			g_Freq;
u8			g_FreqDetected;
bool		g_DoSynch;
u8			g_VersionVDP;
u16			g_Scroll;
bool		g_Initialized = FALSE;
u8			g_StartPage = 0xFF;
u8			g_Continue;
u8			g_ScorePage;

// Audio
u8			g_OptMusicIdx = 0;
u8			g_LastMusicId = 0xFF;

// Gameplay
u8			g_GameMode = MODE_TRAINNNG;
Player		g_Players[PLAYER_MAX];	// Players information
Player		g_TrainPlayer;	// Players information
u8			g_PlayerMax;
Vector		g_BonusPos;				// Bonus information
u8			g_BonusTile;
c8			g_StrBuffer[32];		// String temporary buffer
u8			g_ScreenBuffer[32*24];
u8			g_CurrentPlayer;
u8			g_BonusLen = BONUS_GROWTH;
bool		g_Pause;
u8			g_CollapseTimer;
u8			g_CollapsePhase;
u8			g_CollapseX0;
u8			g_CollapseY0;
u8			g_CollapseX1;
u8			g_CollapseY1;
Player*		g_Winner;				// Winner player index
MenuItemMinMax g_MenuLevelMinMax;

// Solo mode
Vector		g_PlayerStart;
u8			g_BonusNum;
u16			g_TrainScore[TRAIN_LEVEL_MAX];
u16			g_TrainTotal = 0;

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
bool		g_Cheat = FALSE;

// Save
bool		g_PACFound;
u8			g_PACPage = 1;
const c8*   g_PACLoadMsg;
const c8*   g_PACSaveMsg;
const c8*   g_PACEraseMsg;

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
// Check if trigger A is pressed
bool IsInputButton1Pressed()
{
	if (Keyboard_IsKeyPressed(KEY_SPACE))
		return TRUE;

	for(u8 i = 0; i < g_JoyNum; ++i)
		if (NTap_IsPressed(i, NTAP_A))
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

//-----------------------------------------------------------------------------
//
void UpdateInputPush()
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
void UpdateInputPressed()
{
	// Update keyboard entries
	if (g_Input[CTRL_KEY_1] == DIR_MAX)
	{
		if (Keyboard_IsKeyPressed(KEY_LEFT))
			g_Input[CTRL_KEY_1] = DIR_LEFT;
		else if (Keyboard_IsKeyPressed(KEY_RIGHT))
			g_Input[CTRL_KEY_1] = DIR_RIGHT;
		else if (Keyboard_IsKeyPressed(KEY_UP))
			g_Input[CTRL_KEY_1] = DIR_UP;
		else if (Keyboard_IsKeyPressed(KEY_DOWN))
			g_Input[CTRL_KEY_1] = DIR_DOWN;
	}
	if (g_Input[CTRL_KEY_2] == DIR_MAX)
	{
		if (Keyboard_IsKeyPressed(KEY_D))
			g_Input[CTRL_KEY_2] = DIR_LEFT;
		else if (Keyboard_IsKeyPressed(KEY_G))
			g_Input[CTRL_KEY_2] = DIR_RIGHT;
		else if (Keyboard_IsKeyPressed(KEY_R))
			g_Input[CTRL_KEY_2] = DIR_UP;
		else if (Keyboard_IsKeyPressed(KEY_F))
			g_Input[CTRL_KEY_2] = DIR_DOWN;
	}
	// Update joysticks
	for(u8 i = 0; i < g_JoyNum; ++i)
	{
		if (g_Input[i] == DIR_MAX)
		{
			if (NTap_IsPressed(i, NTAP_LEFT))
				g_Input[i] = DIR_LEFT;
			else if (NTap_IsPressed(i, NTAP_RIGHT))
				g_Input[i] = DIR_RIGHT;
			else if (NTap_IsPressed(i, NTAP_UP))
				g_Input[i] = DIR_UP;
			else if (NTap_IsPressed(i, NTAP_DOWN))
				g_Input[i] = DIR_DOWN;
		}
	}
}

//.............................................................................
// AUDIO
//.............................................................................

//-----------------------------------------------------------------------------
// Play the given music
void PlayMusic(u8 id)
{
	if (!g_Option.OptMusic)
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
	AKG_InitSFX((const void*)0xE800);
}

//-----------------------------------------------------------------------------
// Play the given sound effect
void PlaySFX(u8 id)
{
	if (!g_Option.OptSFX)
		return;

	AKG_PlaySFX(id, ARKOS_CHANNEL_C, 0);
}

//.............................................................................
// ...
//.............................................................................

u16 g_AddrVRAM;

//-----------------------------------------------------------------------------
// Load in all 3 screen sections
void VDP_LoadGM2_Pletter(const u8* src)
{
	u16 dst = g_AddrVRAM;
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
	dst += 0x800;
	Pletter_UnpackToVRAM(src, dst);
}

//-----------------------------------------------------------------------------
// Load patterns in all 3 screen sections
void VDP_LoadPattern_GM2_Pletter(const u8* src, u8 offset)
{
	g_AddrVRAM = g_ScreenPatternLow + (offset * 8);
	VDP_LoadGM2_Pletter(src);
}

//-----------------------------------------------------------------------------
// Load colors in all 3 screen sections
void VDP_LoadColor_GM2_Pletter(const u8* src, u8 offset)
{
	g_AddrVRAM = g_ScreenColorLow + (offset * 8);
	VDP_LoadGM2_Pletter(src);
}

//-----------------------------------------------------------------------------
// 
void PrintChr(u8 x, u8 y, c8 chr)
{
	VDP_Poke_GM2(x++, y, chr);
}

//-----------------------------------------------------------------------------
// 
void PrintChrX(u8 x, u8 y, c8 chr, u8 len)
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
// Update the player score on screen
void EraseChar(u8 x, u8 y)
{
	VDP_Poke_GM2(x, y, g_ScreenBuffer[x + (y * 32)]);
}

//-----------------------------------------------------------------------------
// Update the player score on screen
void PrintFrame(u8 y)
{
	// Draw field
	PrintChrY(0,  y, TILE_TREE, 24 - y);
	PrintChrY(31, y, TILE_TREE, 24 - y);
	PrintChrX(1,  y, TILE_TREE, 30);
	PrintChrX(1, 23, TILE_TREE, 30);
}

//-----------------------------------------------------------------------------
// Clear the frame buffer in RAM
void ClearLevel()
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
void DrawLevel()
{
	VDP_WriteVRAM(g_ScreenBuffer, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32 * 24);
}

//-----------------------------------------------------------------------------
// Get tile value from the RAM frame buffer
u8 GetTile(u8 x, u8 y)
{
	return g_ScreenBuffer[x + (y * 32)];
}

//-----------------------------------------------------------------------------
// Draw a tile in he frame buffer in RAM
void DrawTile(u8 x, u8 y, c8 chr)
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
	u8 tile = g_WallData[g_Option.WallOpt];
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
			switch(val & 0x03)
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
void SetScore(Player* ply)
{
	Print_DrawIntAt(ply->ID * 4 + 1, 0, ply->Score);
	EraseChar(g_PrintData.CursorX, g_PrintData.CursorY);
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
	// u8 col = step > 4 ? 2 : step & 0x01;
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
		u8 col = ((g_Frame >> 4) & 0x01) ? COLOR_WHITE : COLOR_LIGHT_RED;
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
	if (g_Option.CtrlTurn == TURN_CUSTOM)
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
	if (lastPly->Score >= g_Option.GameCount)
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
bool CheckDeathmatch(Player* ply, u8 cell)
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
		if (op->Score >= g_Option.GameCount)
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
void CheckSizeMatters()
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
	if (ply->Score >= g_Option.GameCount) // check victory count
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
	g_BonusTile = g_BonusData[g_Option.BonusOpt];
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
	dist = CheckDir(x, y, (ply->Dir + 1) & 0x03 /*% DIR_MAX*/, 3);
	if (dist < 3)
	{
		i8 w = g_DistWeight[dist];
		weight[ACTION_NONE] += w;
		weight[ACTION_LEFT] += w;
	}

	// Check left
	dist = CheckDir(x, y, (ply->Dir + DIR_MAX - 1) & 0x03 /*% DIR_MAX*/, 3);
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
	ply->Turn       = (g_Option.CtrlTurn != TURN_RELATIVE) ? TURN_ABSOLUTE : TURN_RELATIVE;
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
		if ((g_GameMode == MODE_SIZEMATTERS) && (ply->Length > ply->Score))
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
				EraseChar(x, y);
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
				if (!CheckDeathmatch(ply, cell))
					return;
			case MODE_SIZEMATTERS:
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
					DrawTile(x, y, TILE_PREHOLE);
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
				else if (g_GameMode == MODE_SIZEMATTERS)
				{
					loop(i, PLAYER_MAX)
					{
						Player* p2 = &g_Players[i];
						if((p2->ID != ply->ID) && (p2->Score > 0))
						{
							p2->Score--;
							SetScore(p2);
						}
					}
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
	g_Option.GameCount = info->Count;
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
			case START_CTRL_TEST:
				FSM_SetState(&State_CtrlTest);
				break;
			case START_SCORE_BOARD:
				g_ScorePage = 0;
				FSM_SetState(&State_ScoreBoard);
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
		g_Option.Speed = (g_Option.Speed + 1) % SPEED_MAX;
		break;

	case MENU_ACTION_DEC:
		g_Option.Speed = (g_Option.Speed + SPEED_MAX - 1) % SPEED_MAX;
		break;

	case MENU_ACTION_GET:
		switch(g_Option.Speed)
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
	if (g_Frame & 0x07)
		return NULL;

	if (g_MenuItem != 1)
		return "";

	if (g_Frame & 0x07)
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
	if (g_Frame & 0x07)
		return NULL;

	return ScrollString(g_TextCredits, sizeof(g_TextCredits) - 1);
}

//-----------------------------------------------------------------------------
//
void ApplyFreqOption()
{
	if (g_Option.FreqOpt == FREQ_60HZ) 
		g_Freq = FREQ_60HZ;
	else if (g_Option.FreqOpt == FREQ_50HZ)
		g_Freq = FREQ_50HZ;
	else
		g_Freq = g_FreqDetected;
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
		g_Option.FreqOpt = (g_Option.FreqOpt + 1) % FREQ_MAX;
		break;
	case MENU_ACTION_DEC:
		g_Option.FreqOpt = (g_Option.FreqOpt + (FREQ_MAX - 1)) % FREQ_MAX;
		break;
	}

	ApplyFreqOption();
	if (g_Option.FreqOpt == FREQ_60HZ) 
		return "60HZ";
	else if (g_Option.FreqOpt == FREQ_50HZ)
		return "50HZ";
	else //if (g_Option.FreqOpt == FREQ_AUTO) 
	{
		if (g_Freq == FREQ_50HZ)
			return "AUTO (50HZ)";
		else
			return "AUTO (60HZ)";
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//
void ApplyTurnOption()
{
	u8 turn = (g_Option.CtrlTurn == TURN_ABSOLUTE) ? TURN_ABSOLUTE : TURN_RELATIVE;
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
		g_Option.CtrlTurn = (g_Option.CtrlTurn + 1) % TURN_MAX;
		ApplyTurnOption();
		break;
	case MENU_ACTION_DEC:
		g_Option.CtrlTurn = (g_Option.CtrlTurn + (TURN_MAX - 1)) % TURN_MAX;
		ApplyTurnOption();
		break;
	}

	switch (g_Option.CtrlTurn)
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
void ApplyPaletteOption()
{
	switch(g_Option.PalOpt)
	{
	case PAL_CUSTOM:
		VDP_SetPalette((u8*)g_CustomPalette);
		return;
	case PAL_MSX1: 
		VDP_SetMSX1Palette();
		return;
	case PAL_MSX2: 
		VDP_SetDefaultPalette();
		return;
	case PAL_GRASS:
		VDP_SetPalette((u8*)g_GrassPalette);
		return;
	case PAL_GRAY:
		VDP_SetPalette((u8*)g_GrayPalette);
		return;
	}
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
		g_Option.PalOpt = (g_Option.PalOpt + 1) % PAL_MAX;
		break;
	case MENU_ACTION_DEC:
		g_Option.PalOpt = (g_Option.PalOpt + (PAL_MAX - 1)) % PAL_MAX;
		break;
	}

	ApplyPaletteOption();
	switch(g_Option.PalOpt)
	{
	case PAL_CUSTOM:
		return "CUSTOM";
	case PAL_MSX1: 
		return "MSX1";
	case PAL_MSX2: 
		return "MSX2";
	case PAL_GRASS:
		return "GRASS";
	case PAL_GRAY:
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
			TOGGLE(g_Option.OptMusic);
			PlayMusic(g_OptMusicIdx);
			break;
		}
		return g_Option.OptMusic ? "*" : "/";
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
		TOGGLE(g_Option.OptSFX);
		break;
	}
	return g_Option.OptSFX ? "*" : "/";
}

//-----------------------------------------------------------------------------
//
void StoreControllerOption()
{
	loop(i, PLAYER_MAX)
		g_Option.BattleController[i] = g_Players[i].Controller;
	g_Option.TrainController = g_TrainPlayer.Controller;
	g_Option.TrainID = g_TrainPlayer.ID;
}

//-----------------------------------------------------------------------------
//
void ApplyControllerOption()
{
	loop(i, PLAYER_MAX)
		g_Players[i].Controller = g_Option.BattleController[i];
	g_TrainPlayer.Controller = g_Option.TrainController;
	g_TrainPlayer.ID = g_Option.TrainID;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_PAC(u8 op, i8 value)
{
	if(value == 0) // slot
	{
		if(!g_PACFound)
			return "/NOT FOUND!";
		else
		{
			u8 slotId = PAC_GetDefaultSlot(); 
			c8* str = g_StrBuffer;
			*str++ = '*';
			*str++ = 'F';
			*str++ = 'O';
			*str++ = 'U';
			*str++ = 'N';
			*str++ = 'D';
			*str++ = ' ';
			*str++ = '(';
			*str++ = '0' + SLOT_PRIM(slotId);
			if(IS_SLOT_EXP(slotId))
			{
				*str++ = '-';
				*str++ = '0' + SLOT_SEC(slotId);
			}
			*str++ = ')';
			*str = 0;
			return g_StrBuffer;
		}
	}

	if(!g_PACFound)
		return NULL;

	if(value == 1) // load
	{
		switch(op)
		{
		case MENU_ACTION_SET:
		case MENU_ACTION_INC:
		case MENU_ACTION_DEC:
			switch(PAC_Check(g_PACPage - 1))
			{
			case PAC_CHECK_EMPTY:
				g_PACLoadMsg = "EMPTY!";
				break;
			case PAC_CHECK_UNDEF:
			case PAC_CHECK_ERROR:
				g_PACLoadMsg = "UNDEFINE!";
				break;
			case PAC_CHECK_APP:
				PAC_Read(g_PACPage - 1, (u8*)g_Option, sizeof(g_Option));
				ApplyFreqOption();
				ApplyTurnOption();
				ApplyPaletteOption();
				ApplyControllerOption();
				PlayMusic(g_OptMusicIdx);
				g_PACLoadMsg = "LOADED!";
				break;
			} 
			break;
		}
		return g_PACLoadMsg;
	}
	else if(value == 2) // save
	{
		switch(op)
		{
		case MENU_ACTION_SET:
		case MENU_ACTION_INC:
		case MENU_ACTION_DEC:
			StoreControllerOption();
			PAC_Write(g_PACPage - 1, (const u8*)g_Option, sizeof(g_Option));
			g_PACSaveMsg = "SAVED!";
			break;
		}
		return g_PACSaveMsg;
	}
	else if(value == 3) // erase
	{
		switch(op)
		{
		case MENU_ACTION_SET:
		case MENU_ACTION_INC:
		case MENU_ACTION_DEC:
			PAC_Format(g_PACPage - 1);
			g_PACEraseMsg = "FORMATED!";
			break;
		}
		return g_PACEraseMsg;
	}

	return NULL;
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
		g_Option.BonusOpt++;
		if (g_Option.BonusOpt >= sizeof(g_BonusData))
			g_Option.BonusOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_Option.BonusOpt--;
		if (g_Option.BonusOpt == 255)
			g_Option.BonusOpt = sizeof(g_BonusData) - 1;
		break;
	}

	if (g_BonusData[g_Option.BonusOpt] == 0)
		return "RANDOM";

	g_StrBuffer[0] = g_BonusData[g_Option.BonusOpt] + 0x20;
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
		g_Option.WallOpt++;
		if (g_Option.WallOpt >= sizeof(g_WallData))
			g_Option.WallOpt = 0;
		break;
	case MENU_ACTION_DEC:
		g_Option.WallOpt--;
		if (g_Option.WallOpt == 255)
			g_Option.WallOpt = sizeof(g_WallData) - 1;
		break;
	}

	if (g_WallData[g_Option.WallOpt] == 0)
		return "RANDOM";

	g_StrBuffer[0] = g_WallData[g_Option.WallOpt] + 0x20;
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
	case MSXVER_1:  return "MSX1";
	case MSXVER_2:  return "MSX2";
	case MSXVER_2P: return "MSX2+";
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
	if(g_Option.TrainLevel)
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
	g_MenuLevelMinMax.Max = g_Cheat ? TRAIN_LEVEL_MAX : g_Option.HiLevel;
	g_MenuLevelMinMax.Step = 1;

	g_StartPage = MENU_SOLO;
}

//-----------------------------------------------------------------------------
//
void MenuOpen_Control()
{
	g_StartPage = MENU_CONTROL;
}

//-----------------------------------------------------------------------------
//
void MenuOpen_Save()
{
	g_PACLoadMsg = NULL;
	g_PACSaveMsg = NULL;
	g_PACEraseMsg = NULL;
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
		VDP_SetPalette((u8*)g_CustomPalette);
		g_Option.PalOpt = PAL_CUSTOM;
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
		g_Option.HiScore[i] = 0;
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
	VDP_HideAllSprites();
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

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Background
	ClearLevel();
	DrawLevel();

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

	// Initialize sprites data
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_8);
	Pletter_UnpackToVRAM(g_DataSprites, g_SpritePatternLow);
	VDP_HideAllSprites();

	VDP_SetColor(COLOR_LIGHT_YELLOW);

	// Draw field
	PrintFrame(0);

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
	VDP_HideAllSprites();

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
		else if ((g_Option.CtrlTurn == TURN_CUSTOM) && (IsInputUp() || IsInputDown()))
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
void DrawFrame()
{
	DrawTile(0,  1, 0xE9);
	DrawTileX(1,  1, 0xE8, 30);
	DrawTile(31, 1, 0xEA);
	DrawTileY(31, 2, 0xEC, 21);
	DrawTile(31, 23, 0xEE);
	DrawTileX(1, 23, 0xE8, 30);
	DrawTile(0, 23, 0xED);
	DrawTileY(0,  2, 0xEB, 21);

	// Timer board
	if ((g_GameMode == MODE_TRAINNNG) || (g_ModeInfo[g_GameMode].Time))
	{
		DrawTile(14, 23, TILE_CLOCK + 0);
		DrawTile(15, 23, TILE_CLOCK + 1);
		DrawTile(16, 23, TILE_CLOCK + 2);
		DrawTile(17, 23, TILE_CLOCK + 3);
	}
}

//-----------------------------------------------------------------------------
//
void State_BattleStart_Begin()
{
	// PlaySFX(SFX_START);

	VDP_EnableDisplay(FALSE);
	VDP_HideAllSprites();

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawFrame();

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
	for(u8 i = 0; i < g_Option.WallNum; ++i)
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
		// VDP_HideSprite(i);
	}

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
	VDP_HideAllSprites();

	// Initialize timer
	if (g_ModeInfo[g_GameMode].Time)
	{
		if(g_GameMode == MODE_BATTLEROYAL)
			SetTimer(BATTLEROYAL_TIME);
		else
			SetTimer(g_Option.GameCount);
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

	g_Pause = FALSE;
}

//-----------------------------------------------------------------------------
//
void UpdatePause()
{
	if (Keyboard_IsKeyPushed(KEY_P))
	{
		PlaySFX(SFX_SELECT);		
		g_Pause = !g_Pause;
	}
	if(g_Pause && (g_Frame & 0x08))
	{
		VDP_SetSpriteSM1(8,  (u8)(119),      90, 0x1A, COLOR_WHITE);
		VDP_SetSpriteSM1(9,  (u8)(119 + 8),  90, 0x1B, COLOR_WHITE);
		VDP_SetSpriteSM1(10, (u8)(119 + 16), 90, 0x1C, COLOR_WHITE);
	}
	else
	{
		VDP_HideSprite(8);
		VDP_HideSprite(9);
		VDP_HideSprite(10);
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

	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);		
		FSM_SetState(&State_BattleSelect);
	}

	// Update pause status
	UpdatePause();
	if(g_Pause)
		return;

	// Update one of the players
	Player* ply = &g_Players[g_CurrentPlayer];
	UpdatePlayer(ply);
	g_CurrentPlayer++;
	g_CurrentPlayer %= PLAYER_MAX;

	UpdateInputPush();

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
				case MODE_SIZEMATTERS:
					CheckSizeMatters();
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
	VDP_HideAllSprites();

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
	PrintFrame(1);

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

	// Initp layer
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
	VDP_HideAllSprites();

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
				g_Option.TrainLevel = 1;
				g_Option.HiLevel = 1;
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

	g_GameMode = MODE_TRAINNNG;

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawFrame();

	// Draw score board
	Player* ply = &g_TrainPlayer;
	DrawTile(1, 0, 0x42 + g_CharaInfo[ply->ID].TileBase);
	UnpackTrainField(g_Option.TrainLevel - 1);
	DrawLevel();
	
	// Initialize font
	g_PrintData.ScreenWidth = 32;
	g_PrintData.PatternOffset = 0;
	Print_SetFontEx(8, 8, 1, 1, ' ', '_', NULL);
	Print_SetMode(PRINT_MODE_TEXT);
	Print_SetTabSize(3);

	// Draw info
	Print_DrawTextAt(4, 0, "LEVEL:");
	Print_DrawInt(g_Option.TrainLevel);

	Print_DrawTextAt(14, 0, "SCORE:");
	Print_DrawInt(g_TrainTotal);

	// Init player
	g_BonusLen = TRAIN_GROWTH;
	SpawnPlayer(ply);
	ply->PosX = g_PlayerStart.X;
	ply->PosY = g_PlayerStart.Y;
	ply->Dir = DIR_UP;
	ply->Score = 0;
	
	SetTimer(0);
	PlayMusic(g_Option.TrainLevel & 1 ? MUSIC_BATTLE : MUSIC_HURRYUP);

	g_Pause = FALSE;

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_TrainGame_Update()
{
	// Wait V-Synch
	WaitVBlank();

	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}
	if (g_Cheat && Keyboard_IsKeyPushed(KEY_N))
	{
		PlaySFX(SFX_DEATH);		
		FSM_SetState(&State_TrainScore);
		return;
	}

	// Update pause status
	UpdatePause();
	if(g_Pause)
		return;
		
	UpdateInputPush();

	// Timer
	Player* ply = &g_TrainPlayer;
	if(ply->State == STATE_PLAYING)
		IncTimer();

	// Update only every 8th frame
	g_CurrentPlayer++;
	u8 speed = (ply->State == STATE_PLAYING) ? g_Option.Speed : SPEED_NORMAL;
	if(IsInputButton1Pressed())
		speed = SPEED_TURBO;
	if((g_CurrentPlayer < g_SpeedData[speed].Count))
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
void DisplayNewHiScore(u8 id)
{
	u8 x = g_PrintData.CursorX * 8 + 2;
	u8 y = g_PrintData.CursorY * 8 - 1;
	VDP_SetSpriteSM1(id++, x,     y,     0x1D, COLOR_WHITE);
	VDP_SetSpriteSM1(id++, x + 8, y++,   0x1E, COLOR_WHITE);
	VDP_SetSpriteSM1(id++, x + 1, y,     0x1D, COLOR_DARK_YELLOW);
	VDP_SetSpriteSM1(id,   x + 9, y,     0x1E, COLOR_DARK_YELLOW);
}

//-----------------------------------------------------------------------------
//
void State_TrainScore_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_HideAllSprites();

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawLevel();

	// Draw field
	PrintFrame(0);
	PrintChrX(1, 6, TILE_TREE, 30);

	// Compute level score
	bool bNewHiScore = FALSE;
	Player* ply = &g_TrainPlayer;
	u16 score = 0;
	u16 maxScore = g_SpeedData[g_Option.Speed].Score;
	if (ply->Score < maxScore)
		score = maxScore - ply->Score;
	if (score > g_Option.HiScore[g_Option.TrainLevel - 1])
	{
		g_Option.HiScore[g_Option.TrainLevel - 1] = score;
		bNewHiScore = TRUE;
	}
	g_TrainScore[g_Option.TrainLevel - 1] = score;

	// Compute total
	g_TrainTotal = GetTotalTrainingScore(g_TrainScore, g_Option.TrainLevel - 1);
	g_Option.HiTotal = GetTotalTrainingScore(g_Option.HiScore, g_Option.TrainLevel - 1);
	bool bNewHiTotal = g_TrainTotal == g_Option.HiTotal;

	Print_DrawTextAt(9, 3, "LEVEL ");
	Print_DrawInt(g_Option.TrainLevel);
	Print_DrawText(" CLEAR!");

	Print_DrawTextAt(11, 10, "SCORE:");
	Print_DrawIntAt(17, 10, g_TrainScore[g_Option.TrainLevel - 1]);
	Print_DrawTextAt(8, 11, "HI-SCORE:");
	Print_DrawIntAt(17, 11, g_Option.HiScore[g_Option.TrainLevel - 1]);
	if(bNewHiScore)
	{
		DisplayNewHiScore(0);
		Print_DrawTextAt(12, 12, "CODE:");
		ScoreData data;
		data.Level   = g_Option.TrainLevel;
		data.Score   = g_Option.HiScore[g_Option.TrainLevel - 1];
		data.Control = (u8)(data.Level + (data.Score & 0xFF) + (data.Score >> 8));
		Crypt_Encode(&data, sizeof(ScoreData), Mem_GetHeapAddress());
		Print_DrawTextAt(17, 12, (const c8*)Mem_GetHeapAddress());
	}

	Print_DrawTextAt(11, 15, "TOTAL:");
	Print_DrawIntAt(17, 15, g_TrainTotal);
	Print_DrawTextAt(8, 16, "HI-SCORE:");
	Print_DrawIntAt(17, 16, g_Option.HiTotal);
	if(bNewHiTotal)
		DisplayNewHiScore(4);

	// Move to next level
	g_Winner = NULL;
	if(g_Option.TrainLevel < TRAIN_LEVEL_MAX)
		g_Option.TrainLevel++;
	else
		g_Winner = &g_TrainPlayer;
	if (g_Option.TrainLevel > g_Option.HiLevel)
		g_Option.HiLevel = g_Option.TrainLevel;

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
	if (Keyboard_IsKeyPushed(KEY_R))
	{
		PlaySFX(SFX_SELECT);
		g_Option.TrainLevel--;
		FSM_SetState(&State_TrainGame);
	}
	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}
}

//.............................................................................
// CONTROL TEST STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_CtrlTest_Begin()
{
	VDP_EnableDisplay(FALSE);
	VDP_DisableSpritesFrom(0);

	//........................................
	// Load tiles

	// Initialize tiles pattern & color
	VDP_LoadPattern_GM2_Pletter(g_DataSelect_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataSelect_Colors, 0);

	//........................................
	// Draw page

	// Background
	VDP_FillVRAM(0x1C, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32*24);

	// Display control
	u8 x = 1;
	u8 y = 0;
	loop(i, CTRL_PLY_NUM)
	{
		VDP_WriteLayout_GM2(SELECT_FRAME, x, y, 7, 6);
		const u8* bg = SELECT_TEST_BG;
		if((i < CTRL_KEY_1) && (i >= g_JoyNum))
			bg = SELECT_TEST_NONE;
		VDP_WriteLayout_GM2(bg, x + 2, y + 2, 3, 3);
		VDP_WriteLayout_GM2(g_DeviceSelect[i].Default, x, y + 6, 7, 3);
		x += 8;
		if(x > 32)
		{
			y += 8;
			x = 1;
		}
	}

	VDP_EnableDisplay(TRUE);
}


//-----------------------------------------------------------------------------
//
void DisplayArrow(u8 x, u8 y, u8 dir)
{
	switch(dir)
	{
	case DIR_UP:
		x += 3;
		y += 2;
		break;
	case DIR_RIGHT:
		x += 4;
		y += 3;
		break;
	case DIR_DOWN:
		x += 3;
		y += 4;
		break;
	case DIR_LEFT:
		x += 2;
		y += 3;
		break;
	}
	PrintChr(x, y, SELECT_TEST_ARROW[dir]);
}

//-----------------------------------------------------------------------------
//
void State_CtrlTest_Update()
{
	// Wait V-Synch
	WaitVBlank();

	// Update input
	loop(i, CTRL_PLY_NUM)
		g_Input[i] = DIR_MAX;
	UpdateInputPressed();

	// Update control display
	u8 x = 1;
	u8 y = 0;
	loop(i, CTRL_PLY_NUM)
	{
		if((i >= CTRL_KEY_1) || (i < g_JoyNum))
		{
			VDP_WriteLayout_GM2(SELECT_TEST_BG, x + 2, y + 2, 3, 3); // Restore background
			u8 dir = g_Input[i];
			if(dir != DIR_MAX)
				DisplayArrow(x, y, dir);
		}
		x += 8;
		if(x > 32)
		{
			y += 8;
			x = 1;
		}
	}

	// Check for exit input
	if (Keyboard_IsKeyPushed(KEY_ESC))
	{
		PlaySFX(SFX_SELECT);
		FSM_SetState(&State_Title);
	}
}

//.............................................................................
// SCORE BOARD STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_ScoreBoard_Begin()
{
	// Initialize VDP
	VDP_EnableDisplay(FALSE);
	VDP_HideAllSprites();

	// Initialize tiles data
	VDP_LoadPattern_GM2_Pletter(g_DataTiles_Patterns, 0);
	VDP_LoadColor_GM2_Pletter(g_DataTiles_Colors, 0);

	// Draw game field
	ClearLevel();
	DrawLevel();

	// Draw field
	PrintFrame(0);

	Print_DrawTextAt(6, 3, "LVL SCORE CODE");

	// Score board
	u8 y = 5;
	u8 lvl = g_ScorePage * 8;
	loop(i, 8)
	{
		// Level
		Print_DrawIntAt(7, y, lvl + 1);

		// Score
		u16 score = g_Option.HiScore[lvl];
		Print_DrawIntAt(11, y, score);

		// Code
		if(score > 0)
		{
			ScoreData data;
			data.Level   = lvl + 1;
			data.Score   = score;
			data.Control = (u8)(data.Level + (score & 0xFF) + (score >> 8));
			Crypt_Encode(&data, sizeof(ScoreData), Mem_GetHeapAddress());
			Print_DrawTextAt(17, y, (const c8*)Mem_GetHeapAddress());
		}

		y += 2;
		lvl++;
	}

	if(g_ScorePage > 0)
		Print_DrawTextAt(2, 21, "<PREV");
	if(g_ScorePage < 4)
		Print_DrawTextAt(25, 21, "NEXT>");

	VDP_EnableDisplay(TRUE);
}

//-----------------------------------------------------------------------------
//
void State_ScoreBoard_Update()
{
	// Wait V-Synch
	WaitVBlank();

	// Handle input
	if (IsInputRight() && (g_ScorePage < 4))
	{
		g_ScorePage++;
		FSM_SetState(&State_ScoreBoard);
		// State_ScoreBoard_Begin();
	}
	if (IsInputLeft() && (g_ScorePage > 0))
	{
		g_ScorePage--;
		FSM_SetState(&State_ScoreBoard);
		// State_ScoreBoard_Begin();
	}
	if ((IsInputButton1()) || Keyboard_IsKeyPushed(KEY_ESC))
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
	g_Option.HiLevel = TRAIN_LEVEL_MAX;
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

	// Initialise PAC
	g_PACFound = PAC_Initialize();

	// Set crypt module key 
	Crypt_SetKey("Crawlers");

	g_Option.FreqOpt = FREQ_AUTO;
	g_Option.PalOpt = 0;
	g_Option.OptMusic = DEF_MUSIC;
	g_Option.OptSFX = DEF_SFX;
	g_Option.GameCount;
	g_Option.BonusOpt = 3;
	g_Option.WallNum = 0;
	g_Option.WallOpt = 0;
	g_Option.Speed = SPEED_NORMAL;
	g_Option.CtrlTurn = TURN_ABSOLUTE;
	g_Option.TrainLevel = 0;		// Current training level [1~40]
	g_Option.HiLevel = 0;
	// g_Option.HiScore[TRAIN_LEVEL_MAX];
	g_Option.HiTotal = 0;

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