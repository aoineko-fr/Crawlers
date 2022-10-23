// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │   ▄▄▄                ▄▄      
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘                 ▀▀
//  Program template
//─────────────────────────────────────────────────────────────────────────────

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "string.h"

//=============================================================================
// DEFINES
//=============================================================================

// Library's logo
#define MSX_GL "\x02\x03\x04\x05"

//
#define LENGTH_MIN					1
#define LENGTH_MAX					128
#define LENGTH_DEFAULT				5

#define INPUT_AI_EASY				100
#define INPUT_AI_MED				101
#define INPUT_AI_HARD				102

#define TILE_EMPTY					0xF0
#define TILE_SALAD					0xF1
#define TILE_MUSH					0xF2

// Direction define
enum DIRECTION
{
	DIR_UP = 0, // 00
	DIR_RIGHT,  // 01
	DIR_DOWN,   // 10
	DIR_LEFT,   // 11
	DIR_MAX,
};

// Input define
enum INPUT
{
	INPUT_NONE = 0,
	INPUT_RIGHT,
	INPUT_LEFT,
	INPUT_MAX,
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

//
typedef void (*InputCB)(u16 addr);

// Player data structure
struct Player
{
	u8 ID;
	InputCB Input;
	u8 PosX;
	u8 PosY;
	u8 Dir;
	u8 Length;
	u8 Expect;
	u8 Idx;
	u8 Path[LENGTH_MAX]; // ring buffer
	u8 Anim;
};

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts data
#include "font\font_mgl_sample8.h"

// Animation characters
const u8 g_ChrAnim[] = { '-', '/', '|', '\\' };

// Tiles
#include "content\tiles.h"

//
const struct Shapes g_Body[] = 
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
const struct Start g_Starts[] = 
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

//=============================================================================
// MEMORY DATA
//=============================================================================

u8 g_Frame = 0;
c8 g_Buffer[32];
struct Player g_Players[8];
struct Vector g_Salad;
u8 g_PrevRow8 = 0xFF;
u8 g_InKey1;

//=============================================================================
// FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void PrintAt(u8 x, u8 y, const c8* str)
{
	while(*str != 0)
		VDP_Poke_GM2(x++, y, *str++ - ' ');
}

//-----------------------------------------------------------------------------
// 
void PrintChrX(u8 x, u8 y, c8 chr, u8 len)
{
	for(u8 i = 0; i < len; ++i)
		VDP_Poke_GM2(x++, y, chr);
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
void PrintInt(u8 x, u8 y, u8 val)
{
	String_FromUInt8ZT(val, g_Buffer);
	PrintAt(x, y, g_Buffer);
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
void UpdateAI(u16 addr)
{
	struct Player* ply = (struct Player*)addr;
	u8 x = ply->PosX;
	u8 y = ply->PosY;

	u8 rnd = Math_GetRandom8();
	
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
			if(rnd & 0x80)
				ply->Dir++;
			else
				ply->Dir--;
			ply->Dir %= 4;
			return;
		}

		loop--;
	}
}

//-----------------------------------------------------------------------------
//
void UpdateKeyboard(u16 addr)
{
	struct Player* ply = (struct Player*)addr;
}

//-----------------------------------------------------------------------------
//
void UpdateJoystick(u16 addr)
{
	struct Player* ply = (struct Player*)addr;
	if(g_InKey1 == INPUT_RIGHT)
	{
		ply->Dir++;
		ply->Dir %= 4;
	}
	else if(g_InKey1 == INPUT_LEFT)
	{
		ply->Dir--;
		ply->Dir %= 4;
	}
	g_InKey1 = INPUT_NONE;
}

//-----------------------------------------------------------------------------
// 
void InitPlayer(struct Player* ply, u8 id)
{
	const struct Start* start = &g_Starts[id];
	ply->ID     = id;
	ply->Input  = (id == 0) ? UpdateJoystick : UpdateAI;
	ply->PosX   = start->X;
	ply->PosY   = start->Y;
	ply->Dir    = start->Dir;
	ply->Length = LENGTH_MIN;
	ply->Expect = LENGTH_DEFAULT;
	ply->Anim   = 0;
	ply->Idx    = 0;
	for(u8 i = 0; i < LENGTH_MIN; ++i)
		ply->Path[i] = start->Dir;
}

//-----------------------------------------------------------------------------
//
void DrawPlayer(struct Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;

	if((ply->Length < ply->Expect) && (ply->Length < LENGTH_MAX))
		ply->Length++;
	else if((ply->Length > ply->Expect) && (ply->Length > LENGTH_MIN))
		ply->Length--;

	for(u8 i = 0; i < ply->Length + 1; ++i)
	{
		u8 tile;
		// Head
		if(i == 0)
		{
			tile = 0x40 + ply->Path[idx];
		}
		// Tail
		else if(i == ply->Length - 1)
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			tile = 0x4C + ply->Path[prev] * 2;
			if(ply->Anim & 1)
				tile++;
		}
		// Clear
		else if(i == ply->Length)
			tile = TILE_EMPTY;
		// Body
		else
		{
			u8 prev = idx - 1;
			prev  %= LENGTH_MAX;
			const struct Shapes* vec = &g_Body[ply->Path[prev] + (ply->Path[idx] << 2)];
			if(idx & 1)
				tile = vec->A;
			else
				tile = vec->B;
		}
		VDP_Poke_GM2(x, y, (tile == TILE_EMPTY) ? TILE_EMPTY : tile + (20 * ply->ID));

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
void ClearPlayer(struct Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	u8 idx = ply->Idx;

	for(u8 i = 0; i < ply->Length; ++i)
	{
		VDP_Poke_GM2(x, y, TILE_EMPTY);

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
}

//-----------------------------------------------------------------------------
// Program entry point
void MovePlayer(struct Player* ply)
{
	u8 x = ply->PosX;
	u8 y = ply->PosY;
	// Move
	switch(ply->Dir)
	{
	case DIR_UP:	y--; break;
	case DIR_RIGHT:	x++; break;
	case DIR_DOWN:	y++; break;
	case DIR_LEFT:	x--; break;
	}

	u8 cell = VDP_Peek_GM2(x, y);
	switch(cell)
	{
	case TILE_SALAD:
		ply->Expect += 5;
		SpawnSalad();
	case TILE_EMPTY:
		ply->PosX = x;
		ply->PosY = y;
		ply->Idx--;
		ply->Idx %= LENGTH_MAX;
		ply->Path[ply->Idx] = ply->Dir;
		DrawPlayer(ply);
		break;
	default:
		ClearPlayer(ply);
		InitPlayer(ply, ply->ID);
	}
}

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_EnableVBlank(TRUE);
	VDP_ClearVRAM();
	VDP_SetColor(COLOR_LIGHT_YELLOW);

	VDP_LoadPattern_GM2(g_DataTiles_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataTiles_Colors, 255, 0);

	VDP_FillScreen_GM2(TILE_EMPTY);
	for(u8 i = 0; i < 8; ++i)
	{
		VDP_Poke_GM2(i * 4, 0, 0x42 + 20 * i);
		PrintChrX(i * 4 + 1, 0, '0'-' ', 2);
	}
	// Up
	VDP_Poke_GM2(0,  1, 0xE4);
	VDP_Poke_GM2(31, 1, 0xE6);
	PrintChrX(1,  1, 0xE5, 30);
	// Sides
	PrintChrY(0,  2, 0xE7, 21);
	PrintChrY(31, 2, 0xE8, 21);
	// Down
	VDP_Poke_GM2(0, 23, 0xE9);
	VDP_Poke_GM2(31, 23, 0xEB);
	PrintChrX(1, 23, 0xEA, 30);

	SpawnSalad();

	for(u8 i = 0; i < 8; ++i)
		InitPlayer(&g_Players[i], i);

	u8 upply = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(g_InKey1 == INPUT_NONE)
		{
			u8 row8 = Keyboard_Read(8);
			if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))
			{
				g_InKey1 = INPUT_RIGHT;
			}
			if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))
			{
				g_InKey1 = INPUT_LEFT;
			}
			g_PrevRow8 = row8;
		}

		Halt();

		struct Player* ply = &g_Players[upply];
		ply->Input((u16)ply);
		MovePlayer(ply);
		upply++;
		upply %= 8;

		VDP_Poke_GM2(g_Salad.X, g_Salad.Y, TILE_SALAD);

		g_Frame++;
	}

	Bios_Exit(0);
}