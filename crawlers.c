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
#include "string.h"
#include "fsm.h"
#include "game_menu.h"
#include "device\ninjatap.h"

// Crawlers
#include "crawlers.h"

//=============================================================================
// DEFINES
//=============================================================================

#define MSXGL						"[\\]^"

// Configuration
#define MSX2_ENHANCE				1

// Prototypes
void State_Init_Begin();
void State_Init_Update();

void State_Title_Begin();
void State_Title_Update();

void State_Menu_Begin();
void State_Menu_Update();

void State_Select_Begin();
void State_Select_Update();

void State_Game_Begin();
void State_Game_Update();

const c8* MenuAction_Start(u8 op, i8 value);
const c8* MenuAction_Mode(u8 op, i8 value);
const c8* MenuAction_Exit(u8 op, i8 value);

void InitPlayer(Player* ply, u8 id, bool bRespawn);
void DrawPlayer(Player* ply);
void ClearPlayer(Player* ply);
void UpdatePlayer(Player* ply);

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Tiles
#include "content\tiles.h"

// Sprites
#include "content\sprites.h"

// Menu
#include "content\select.h"
#include "content\face1.h"
#include "content\face2.h"

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
const Character g_Chara[] =
{
	{ "Chara 01", 0*20, 0    },
	{ "Chara 02", 2*20, 1    },
	{ "Chara 03", 5*20, 0xFF },
	{ "Chara 04", 6*20, 0xFF },
	{ "Chara 05", 1*20, 2    },
	{ "Chara 06", 3*20, 3    },
	{ "Chara 07", 4*20, 0xFF },
	{ "Chara 08", 7*20, 0xFF },
};

//
const Start g_Starts[] =
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

#if (MSX2_ENHANCE)
// Alternative
const u16 g_MSX2Palette[15] = {
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
#endif

//
const c8 g_TitleTile[] = {
	0xC0, 0xBD, 0xC1, 0x84, 0x80, 0x85, 0x48, 0x45, 0x49, 0xB5, 0x00, 0xA4, 0xDD, 0x00, 0x00, 0x5C, 0x59, 0x66, 0x98, 0x94, 0x99, 0x70, 0x6C, 0x69,
	0xBF, 0x00, 0xBA, 0x82, 0x7F, 0x87, 0x47, 0x00, 0x42, 0xAB, 0x00, 0xAB, 0xD3, 0x00, 0x00, 0x56, 0x00, 0x00, 0x96, 0x93, 0x9B, 0x6E, 0x00, 0x00,
	0xBE, 0x00, 0x00, 0x83, 0x7F, 0x85, 0x46, 0x43, 0x49, 0xAA, 0x00, 0xAA, 0xD2, 0x00, 0x00, 0x5C, 0x55, 0x00, 0x97, 0x93, 0x99, 0x72, 0x6D, 0x71,
	0xBF, 0x00, 0xC9, 0x82, 0x00, 0x83, 0x47, 0x00, 0x47, 0xAB, 0xA4, 0xAB, 0xD3, 0x00, 0x00, 0x5B, 0x00, 0x00, 0x96, 0x00, 0x96, 0x00, 0x00, 0x6F,
	0xC2, 0xBC, 0xC3, 0x89, 0x00, 0x88, 0x4D, 0x00, 0x4C, 0xAE, 0xAF, 0xB1, 0xD6, 0xD0, 0xCD, 0x5E, 0x58, 0x67, 0x9C, 0x00, 0x9D, 0x77, 0x6C, 0x73,
};

extern i16 g_Test;

//
const MenuItem g_MenuMain[] = {
	{ "PLAY",                MENU_ITEM_GOTO, NULL, MENU_PLAY },
	{ "OPTIONS",             MENU_ITEM_GOTO, NULL, MENU_OPTION },
	{ "CREDITS",             MENU_ITEM_GOTO, NULL, MENU_CREDIT },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "EXIT",                MENU_ITEM_ACTION, MenuAction_Exit, 0 },
};

//
const MenuItem g_MenuPlay[] = {
	{ "START",               MENU_ITEM_ACTION, MenuAction_Start, 0 },
	{ "MODE",                MENU_ITEM_ACTION, MenuAction_Mode, 0 },
	{ "COUNT",               MENU_ITEM_INT, &g_GameCount, 1 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const MenuItem g_MenuOption[] = {
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const MenuItem g_MenuCredit[] = {
	{ "CODE:  AOINEKO",      MENU_ITEM_TEXT, NULL, 0 },
	{ "GFX:   AOINEKO",      MENU_ITEM_TEXT, NULL, 0 },
	{ "MUSIC: ???",          MENU_ITEM_TEXT, NULL, 0 },
	{ "SFX:   AYFX SAMPLE",  MENU_ITEM_TEXT, NULL, 0 },
	{ NULL,                  MENU_ITEM_EMPTY, NULL, 0 },
	{ "BACK",                MENU_ITEM_GOTO, NULL, MENU_MAIN },
};

//
const Menu g_Menus[MENU_MAX] =
{
	{ NULL, g_MenuMain,   numberof(g_MenuMain) },
	{ NULL, g_MenuPlay,   numberof(g_MenuPlay) },
	{ NULL, g_MenuOption, numberof(g_MenuOption) },
	{ NULL, g_MenuCredit, numberof(g_MenuCredit) },
};

const FSM_State State_Init =	{ 0, State_Init_Begin,		State_Init_Update,		NULL };
const FSM_State State_Title =	{ 0, State_Title_Begin,		State_Title_Update,		NULL };
const FSM_State State_Menu =	{ 0, State_Menu_Begin,		State_Menu_Update,		NULL };
const FSM_State State_Select =	{ 0, State_Select_Begin,	State_Select_Update,	NULL };
const FSM_State State_Game =	{ 0, State_Game_Begin,		State_Game_Update,		NULL };

//=============================================================================
// MEMORY DATA
//=============================================================================

// System
u8 g_VBlank = 0;					// Vertical-synchronization flag
u8 g_Frame = 0;						// Frame counter

// Gameplay
u8 g_GameMode = MODE_BATTLEROYAL;
u8 g_GameCount = 3;
Player g_Players[8];			// Players information
Vector g_Salad;				// Salad information
c8 g_StrBuffer[32];					// String temporary buffer
u8 g_ScreenBuffer[32*24];
u8 g_CurrentPlayer;

// Input
u8 g_JoyNum;
u8 g_PrevRow3 = 0xFF;
u8 g_PrevRow8 = 0xFF;
u8 g_Input[INPUT_NUM];

#if (MSX2_ENHANCE)
u8 g_VersionVDP;
#endif

i16 g_Test = 0;

//=============================================================================
// FUNCTIONS
//=============================================================================

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
// 
void DrawTile(u8 x, u8 y, c8 chr)
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
void SetScore(Player* ply)
{
	Print_DrawIntAt(ply->ID * 4 + 1, 0, ply->Score);
}

//-----------------------------------------------------------------------------
//
void CheckBattleRoyal()
{
	Player* lastPly = NULL;
	for(u8 i = 0; i < 8; ++i)
	{
		if(g_Players[i].State != STATE_NONE)
		{
			if(lastPly)
				return;
			lastPly = &g_Players[i];
		}
	}

	lastPly->Score++;
	SetScore(lastPly);
	ClearPlayer(lastPly);

	for(u8 i = 0; i < 8; ++i)
		InitPlayer(&g_Players[i], i, TRUE);
}

//-----------------------------------------------------------------------------
// 
void SpawnSalad()
{
	u8 rnd = Math_GetRandom8();
	u8 x = 8 + rnd % 16;
	u8 y = 8 + (rnd >> 4) % 8;
	while(VDP_Peek_GM2(x, y) != TILE_EMPTY)
		x++;
	g_Salad.X = x;
	g_Salad.Y = y;
	VDP_Poke_GM2(x, y, TILE_SALAD);
}

//-----------------------------------------------------------------------------
//
void UpdateAI(Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;

	u8 rnd = Math_GetRandom8();

	// Check collision
	u8 loop = 3;
	while(loop != 0)
	{
		switch(ply->Dir)
		{
		case DIR_UP:	y--; break;
		case DIR_RIGHT:	x++; break;
		case DIR_DOWN:	y++; break;
		case DIR_LEFT:	x--; break;
		}

		u8 cell = VDP_Peek_GM2(x, y);
		if(cell < 0xF0)
		{
			if(rnd & 0x80) // 50%
				ply->Dir++;
			else
				ply->Dir--;
			ply->Dir %= 4;
			return;
		}

		loop--;
	}

	// Seek salad
	if((rnd & 0x60) == 0) // 25%
	{
		switch(ply->Dir)
		{
		case DIR_UP:
		case DIR_DOWN:
			if(g_Salad.X > ply->PosX)
				ply->Dir = DIR_RIGHT;
			else if(g_Salad.X < ply->PosX)
				ply->Dir = DIR_LEFT;
			return;

		case DIR_RIGHT:
		case DIR_LEFT:
			if(g_Salad.Y > ply->PosY)
				ply->Dir = DIR_DOWN;
			else if(g_Salad.Y < ply->PosY)
				ply->Dir = DIR_UP;
			return;
		}
	}
}

//-----------------------------------------------------------------------------
//
void UpdatePlayerInput(Player* ply)
{
	if(g_Input[ply->Input] == ACTION_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if(g_Input[ply->Input] == ACTION_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
	g_Input[ply->Input] = ACTION_NONE;
}

//-----------------------------------------------------------------------------
// 
void InitPlayer(Player* ply, u8 id, bool bRespawn)
{
	const Start* start = &g_Starts[id];
	ply->ID     = id;
	ply->PosX   = start->X;
	ply->PosY   = start->Y;
	ply->Dir    = start->Dir;
	ply->Length = LENGTH_MIN;
	ply->Expect = LENGTH_DEFAULT;
	ply->Anim   = 0;
	ply->Idx    = 0;
	ply->State  = STATE_INIT;
	ply->Timer  = 0;
	if(!bRespawn)
		ply->Score  = 0;
	for(u8 i = 0; i < LENGTH_MIN; ++i)
		ply->Path[i] = start->Dir;

	switch(id)
	{
	case 0:
		ply->Action = UpdatePlayerInput;
		ply->Input  = INPUT_KEY_1;
		break;
	case 1:
		ply->Action = UpdatePlayerInput;
		ply->Input  = INPUT_KEY_2;
		break;
	default:
		ply->Action = UpdateAI;
		ply->Input  = INPUT_AI_MED;
		break;
	};

	SetScore(ply);
}

//-----------------------------------------------------------------------------
//
void DrawPlayer(Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;
	u8 baseTile = g_Chara[ply->ID].TileBase;
	bool bGrow = FALSE;
	bool bReduce = FALSE;

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
	else if((ply->Length > ply->Expect) && (ply->Length > LENGTH_MIN))
	{
		ply->Length--;
		bReduce = TRUE;
	}

	for(u8 i = 0; i < ply->Length + 1; ++i)
	{
		// Head
		if(i == 0)
		{
			u8 tile = 0x40 + ply->Path[idx];
			VDP_Poke_GM2(x, y, tile + baseTile);

			// Sprite
			const Character* chr = &g_Chara[ply->ID];
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
	const Character* chr = &g_Chara[ply->ID];
	if(chr->Sprite != 0xFF)
		VDP_HideSprite(chr->Sprite);

	ply->State = STATE_NONE;
}

//-----------------------------------------------------------------------------
// Program entry point
void UpdatePlayer(Player* ply)
{
	if(ply->Input == INPUT_NONE)
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
		if(cell < 0xF0)
		{
			ClearPlayer(ply);
			switch(g_GameMode)
			{
			case MODE_BATTLEROYAL:
				CheckBattleRoyal();
				return;
			case MODE_DEATHMATCH:
			case MODE_SIZEMATTER:
				InitPlayer(ply, ply->ID, TRUE);
				return;
			};
		}
		else
		{
			switch(cell)
			{
			case TILE_SALAD:
				ply->Expect += 5;
				SpawnSalad();
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
void VBlankHook()
{
	g_VBlank = 1;
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
	while(g_VBlank == 0) {}
	g_VBlank = 0;
	g_Frame++;
}

//=============================================================================
// MENU
//=============================================================================


//-----------------------------------------------------------------------------
//
const c8* MenuAction_Start(u8 op, i8 value)
{
	if(op == MENU_ACTION_SET)
		FSM_SetState(&State_Select);
	return NULL;
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Mode(u8 op, i8 value)
{
	switch(op)
	{
	case MENU_ACTION_SET:
	case MENU_ACTION_INC:
		if(g_GameMode < MODE_MAX-1)
			g_GameMode++;
		else
			g_GameMode = 0;
		break;

	case MENU_ACTION_DEC:
		if(g_GameMode > 0)
			g_GameMode--;
		else
			g_GameMode = MODE_MAX-1;
		break;

	case MENU_ACTION_GET:
		switch(g_GameMode)
		{
		case MODE_BATTLEROYAL: return "BATTLE ROYAL";
		case MODE_DEATHMATCH:  return "DEATH MATCH";
		case MODE_SIZEMATTER:  return "SIZE MATTER";
		};
	};
	return "";
}

//-----------------------------------------------------------------------------
//
const c8* MenuAction_Exit(u8 op, i8 value)
{
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
	#if (MSX2_ENHANCE)
	if(Keyboard_IsKeyPressed(KEY_1))
		g_VersionVDP = VDP_VERSION_TMS9918A;
	else if(Keyboard_IsKeyPressed(KEY_2))
		g_VersionVDP = VDP_VERSION_V9938;
	else
		g_VersionVDP = VDP_GetVersion();

	if(g_VersionVDP > VDP_VERSION_TMS9918A)
		VDP_SetPalette((u8*)g_MSX2Palette);
	#endif

	// Initialize VDP
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_ClearVRAM();
	VDP_SetColor(COLOR_LIGHT_YELLOW);

	// Set VBlank hook
	VDP_EnableVBlank(TRUE);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	// Initialize Joystick and/or Ninja Tap
	NTap_Check();
	g_JoyNum = NTap_GetPortNum();
}

//-----------------------------------------------------------------------------
//
void State_Init_Update()
{
	FSM_SetState(&State_Title);
}

//.............................................................................
// TITLE MENU STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Title_Begin()
{
	VDP_ClearVRAM();

	// Initialize sprites data
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_8);
	VDP_LoadSpritePattern(g_DataSprites, 0, sizeof(g_DataSprites) / 8);

	// Initialize tiles data
	VDP_LoadPattern_GM2(g_DataTiles_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataTiles_Colors, 255, 0);

	VDP_WriteLayout_GM2(g_TitleTile, 4, 3, 24, 5);

	// Draw field
	PrintChr(0,  0, 0xE8);
	PrintChr(31, 0, 0xEA);
	PrintChrX(1, 0, 0xE9, 30);
	PrintChrY(0, 1, 0xEB, 22);
	PrintChrY(31,1, 0xEC, 22);
	PrintChr(0, 23, 0xED);
	PrintChr(31, 23, 0xEF);
	PrintChrX(1, 23, 0xEE, 30);

	// Initialize font
	g_PrintData.ScreenWidth = 32;
	g_PrintData.PatternOffset = 0;
	Print_SetFontEx(8, 8, 1, 1, ' ', '_', NULL);
	Print_SetMode(PRINT_MODE_TEXT);
	Print_SetTabSize(3);

	Print_DrawTextAt(2, 21, "PIXEL PHENIX 2023   " MSXGL);

	// Initialize sprites data
	VDP_SetSpriteSM1(0, (u8)(26 * 8), (u8)(21 * 8 - 1), 12, COLOR_DARK_GREEN);
	VDP_SetSpriteSM1(1, (u8)(27 * 8), (u8)(21 * 8 - 1), 13, COLOR_DARK_GREEN);
	VDP_SetSpriteSM1(2, (u8)(28 * 8), (u8)(21 * 8 - 1), 14, COLOR_DARK_GREEN);
	VDP_SetSpriteSM1(3, (u8)(29 * 8), (u8)(21 * 8 - 1), 15, COLOR_DARK_GREEN);
	VDP_DisableSpritesFrom(4);
}

//-----------------------------------------------------------------------------
//
void State_Title_Update()
{
	WaitVBlank();

	Print_DrawTextAt(11, 14, (g_Frame & 0x10) ? "PRESS SPACE" : "           ");

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
	// Initialize menu
	Menu_Initialize(g_Menus);
	Menu_DrawPage(MENU_MAIN);
}

//-----------------------------------------------------------------------------
//
void State_Menu_Update()
{
	// Update menu
	WaitVBlank();
	Menu_Update();
}

//.............................................................................
// SELECT STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Select_Begin()
{
	VDP_DisableSpritesFrom(0);

	// Initialize tiles data
	VDP_LoadPattern_GM2(g_DataSelect_Patterns, sizeof(g_DataSelect_Patterns) / 8, 0);
	VDP_LoadColor_GM2(g_DataSelect_Colors, sizeof(g_DataSelect_Colors) / 8, 0);
	VDP_WriteLayout_GM2(g_DataSelect_Names, 0, 0, 32, 24);

	// Upper part portaits
	u16 dst = g_ScreenPatternLow + (128 * 8);
	VDP_WriteVRAM(g_DataFace1_Patterns, dst, g_ScreenPatternHigh, sizeof(g_DataFace1_Patterns));
	dst += 0x800;
	VDP_WriteVRAM(g_DataFace1_Patterns, dst, g_ScreenPatternHigh, sizeof(g_DataFace1_Patterns));
	dst += 0x800;
	VDP_WriteVRAM(g_DataFace2_Patterns, dst, g_ScreenPatternHigh, sizeof(g_DataFace2_Patterns));

	// Lower part portaits
	dst = g_ScreenColorLow + (128 * 8);
	VDP_WriteVRAM(g_DataFace1_Colors, dst, g_ScreenColorHigh, sizeof(g_DataFace1_Colors));
	dst += 0x800;
	VDP_WriteVRAM(g_DataFace1_Colors, dst, g_ScreenColorHigh, sizeof(g_DataFace1_Colors));
	dst += 0x800;
	VDP_WriteVRAM(g_DataFace2_Colors, dst, g_ScreenColorHigh, sizeof(g_DataFace2_Colors));
}

//-----------------------------------------------------------------------------
//
void State_Select_Update()
{
	if(Keyboard_IsKeyPressed(KEY_SPACE))
		FSM_SetState(&State_Game);
}

//.............................................................................
// GAME STATE
//.............................................................................

//-----------------------------------------------------------------------------
//
void State_Game_Begin()
{
	// Initialize tiles data
	VDP_LoadPattern_GM2(g_DataTiles_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataTiles_Colors, 255, 0);

	// Initialize sprites data
	VDP_DisableSpritesFrom(4);

	// Draw game field
	Mem_Set(TILE_EMPTY, g_ScreenBuffer, 32*24);
	// Up
	DrawTile(0,  1, 0xE8);
	DrawTile(31, 1, 0xEA);
	DrawTileX(1,  1, 0xE9, 30);
	// Sides
	DrawTileY(0,  2, 0xEB, 21);
	DrawTileY(31, 2, 0xEC, 21);
	// Down
	DrawTile(0, 23, 0xED);
	DrawTile(31, 23, 0xEF);
	DrawTileX(1, 23, 0xEE, 30);
	// Draw score board
	for(u8 i = 0; i < 8; ++i)
	{
		DrawTile(i * 4, 0, 0x42 + g_Chara[i].TileBase);
	}
	// Draw pre-hole
	for(u8 i = 0; i < 8; ++i)
	{
		const Start* start = &g_Starts[i];
		DrawTile(start->X, start->Y, TILE_PREHOLE);
	}

	// Copy screen buffer to VRAM
	VDP_WriteVRAM(g_ScreenBuffer, g_ScreenLayoutLow, g_ScreenLayoutHigh, 32*24);

	// Initialize Salad
	SpawnSalad();

	for(u8 i = 0; i < 8; ++i)
		InitPlayer(&g_Players[i], i, FALSE);

	g_CurrentPlayer = 0;
}

//-----------------------------------------------------------------------------
//
void State_Game_Update()
{
	WaitVBlank(); // Wait V-Synch

	// Update one of the players
	Player* ply = &g_Players[g_CurrentPlayer];
	UpdatePlayer(ply);
	g_CurrentPlayer++;
	g_CurrentPlayer %= 8;

	VDP_Poke_GM2(g_Salad.X, g_Salad.Y, TILE_SALAD);

	// Update keyboard entries
	if(g_Input[INPUT_KEY_1] == ACTION_NONE)
	{
		u8 row8 = Keyboard_Read(8);
		if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))
			g_Input[INPUT_KEY_1] = ACTION_LEFT;
		else if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))
			g_Input[INPUT_KEY_1] = ACTION_RIGHT;
		g_PrevRow8 = row8;
	}
	if(g_Input[INPUT_KEY_2] == ACTION_NONE)
	{
		u8 row3 = Keyboard_Read(3);
		if(IS_KEY_PRESSED(row3, KEY_D) && !IS_KEY_PRESSED(g_PrevRow3, KEY_D))
			g_Input[INPUT_KEY_2] = ACTION_LEFT;
		else if(IS_KEY_PRESSED(row3, KEY_G) && !IS_KEY_PRESSED(g_PrevRow3, KEY_G))
			g_Input[INPUT_KEY_2] = ACTION_RIGHT;
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
		FSM_SetState(&State_Title);
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