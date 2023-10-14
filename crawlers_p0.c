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
#include "crawlers.h"

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Tiles
#include "content/tiles.h"
#include "content/logo_tile.h"

// Sprites
#include "content/sprites.h"
#include "content/logo_sprt.h"

// Menu
#include "content/select.h"
#include "content/face1.h"
#include "content/face2.h"

// Music
#include "content/music_empty.h"
#include "content/music_intro.h"
#include "content/music_game.h"
#include "content/music_hurry.h"
#include "content/music_victory.h"
#include "content/sfx.h"

// Solo levels
#include "content/level/level001.h"
#include "content/level/level002.h"
#include "content/level/level003.h"
#include "content/level/level004.h"
#include "content/level/level005.h"
#include "content/level/level006.h"
#include "content/level/level007.h"
#include "content/level/level008.h"
#include "content/level/level009.h"
#include "content/level/level010.h"
#include "content/level/level011.h"
#include "content/level/level012.h"
#include "content/level/level013.h"
#include "content/level/level014.h"
#include "content/level/level015.h"
#include "content/level/level016.h"
#include "content/level/level017.h"
#include "content/level/level018.h"
#include "content/level/level019.h"
#include "content/level/level020.h"
#include "content/level/level021.h"
#include "content/level/level022.h"
#include "content/level/level023.h"
#include "content/level/level024.h"
#include "content/level/level025.h"
#include "content/level/level026.h"
#include "content/level/level027.h"
#include "content/level/level028.h"
#include "content/level/level029.h"
#include "content/level/level030.h"
#include "content/level/level031.h"
#include "content/level/level032.h"
#include "content/level/level033.h"
#include "content/level/level034.h"
#include "content/level/level035.h"
#include "content/level/level036.h"
#include "content/level/level037.h"
#include "content/level/level038.h"
#include "content/level/level039.h"
#include "content/level/level040.h"


// Custom palette
const u16 g_CustomPalette[15] =
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

// Alternative palette
const u16 g_GrassPalette[15] =
{
    RGB16(0, 0, 0), // black                RGB16(0, 0, 0),
    RGB16(3, 3, 2), // *dark yellow         RGB16(1, 5, 1), New yellow
    RGB16(6, 6, 4), // *light yellow        RGB16(3, 6, 3), New yellow
    RGB16(1, 1, 5), // dark blue            RGB16(2, 2, 6),
    RGB16(3, 3, 6), // light blue           RGB16(3, 3, 7),
    RGB16(5, 1, 1), // dark red             RGB16(5, 2, 2),
    RGB16(4, 4, 7), // *cyan                RGB16(2, 6, 7),
    RGB16(6, 3, 3), // *medium red          RGB16(6, 2, 2),
    RGB16(7, 4, 4), // *light red           RGB16(6, 3, 3),
    RGB16(2, 5, 2), // medium green         RGB16(5, 5, 2), New green
    RGB16(3, 6, 3), // light green          RGB16(6, 6, 3), New green
    RGB16(2, 2, 1), // *dark yellow         RGB16(1, 4, 1), New yellow
    RGB16(5, 2, 4), // *magenta             RGB16(5, 2, 5),
    RGB16(5, 5, 5), // gray                 RGB16(5, 5, 5),
    RGB16(7, 7, 7)  // white                RGB16(7, 7, 7) 
};

// Gray scale palette
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

//
const u8 g_TitleTile[] =
{
	0x70, 0x6D, 0x71, 0xD4, 0xD0, 0xD5, 0x48, 0x45, 0x49, 0x8D, 0x00, 0x7C, 0xA1, 0x00, 0x00, 0xC0, 0xBD, 0xCA, 0xAC, 0xA8, 0xAD, 0x5C, 0x58, 0x55, 
	0x6F, 0x00, 0x6A, 0xD2, 0xCF, 0xD7, 0x47, 0x00, 0x42, 0x83, 0x00, 0x83, 0x97, 0x00, 0x00, 0xBA, 0x00, 0x00, 0xAA, 0xA7, 0xAF, 0x5A, 0x00, 0x00, 
	0x6E, 0x00, 0x00, 0xD3, 0xCF, 0xD5, 0x46, 0x43, 0x49, 0x82, 0x00, 0x82, 0x96, 0x00, 0x00, 0xC0, 0xB9, 0x00, 0xAB, 0xA7, 0xAD, 0x5E, 0x59, 0x5D, 
	0x6F, 0x00, 0x79, 0xD2, 0x00, 0xD3, 0x47, 0x00, 0x47, 0x83, 0x7C, 0x83, 0x97, 0x00, 0x00, 0xBF, 0x00, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0x00, 0x5B, 
	0x72, 0x6C, 0x73, 0xD9, 0x00, 0xD8, 0x4D, 0x00, 0x4C, 0x86, 0x87, 0x89, 0x9A, 0x94, 0x91, 0xC2, 0xBC, 0xCB, 0xB0, 0x00, 0xB1, 0x63, 0x58, 0x5F, 
};

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
const SelectSlot g_BattleSelectSlot[10] =
{	//                             L   R   U   D
	{ {  22,  53-8 }, {  58,  89-8 }, -1,  1,  8,  4 }, // 0 - Face 1
	{ {  78,  53-8 }, { 114,  89-8 },  0,  2,  8,  5 }, // 1 - Face 2
	{ { 134,  53-8 }, { 170,  89-8 },  1,  3,  9,  6 }, // 2 - Face 3
	{ { 190,  53-8 }, { 226,  89-8 },  2, -1,  9,  7 }, // 3 - Face 4
	{ {  22,  125 },  {  58,  161 },  -1,  5,  0, -1 }, // 4 - Face 5
	{ {  78,  125 },  { 114,  161 },   4,  6,  1, -1 }, // 5 - Face 6
	{ { 134,  125 },  { 170,  161 },   5,  7,  2, -1 }, // 6 - Face 7
	{ { 190,  125 },  { 226,  161 },   6, -1,  3, -1 }, // 7 - Face 8
	{ {  22,  15-8 }, {  58,  22-8 }, -1,  9, -1,  0 }, // 8 - Start
	{ { 198,  15-8 }, { 226,  22-8 },  8, -1, -1,  3 }, // 9 - Exit
};

// Cursor offset animation
const u8 g_CursorAnim[8] = { 1, 1, 2, 1, 1, 0, 0, 0 };

// Hole tile animation
const u8 g_HoleAnim[4] = { TILE_PREHOLE, (u8)(TILE_PREHOLE+1), (u8)(TILE_PREHOLE+2), TILE_HOLE };

// Controller binding
const CtrlBind g_CtrlBind[14] =
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
// CRYPT MODULE
//=============================================================================

#include "crypt.h"

//-----------------------------------------------------------------------------
// CONST DATA
//-----------------------------------------------------------------------------

// Default string containing the 32 valid characters
const c8 g_CryptDefaultMap[] = "HJ48CKP9AWBRSDT3MYLQ0FG1NUE7X562";

// Default bit-field coding table
const u16 g_CryptDefaultCode[8] =
{ //     43210   43210
	0b0000000000001000, // 0
	0b0000100000000100, // 1
	0b0000010000000000, // 2
	0b0000000000000001, // 3
	0b0000000000010000, // 4
	0b0000001000000000, // 5
	0b0001000100000000, // 6
	0b0000000000000010, // 7
};

const u8 g_ShiftTable[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

//-----------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------

// 
const c8* g_CryptKey = NULL;

// String containing the 32 valid characters
const c8* g_CryptMap = g_CryptDefaultMap;

// Bit field coding table
const u16* g_CryptCode = g_CryptDefaultCode;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Encrypt data
// Note: Destination buffer size must be: input data size x 2 + 1
bool Crypt_Encode(const void* data, u8 size, c8* str)
{
	if (!g_CryptKey)
		return FALSE;

	const c8* key = g_CryptKey;
	const u8* ptr = (const u8*)data;

	loop(i, size)
	{
		u8 val = *ptr++;
		val += *key;

		u8 cnt = *key++;
		if (!*key)
			key = g_CryptKey;

		u16 bits = 0;
		u8 flag = 0x01;
		loop(j, 8)
		{
			if (val & flag)
				bits |= g_CryptCode[cnt & 0x07];
			cnt++;
			flag <<= 1;
		}
		*str++ = g_CryptMap[bits & 0x00FF];
		*str++ = g_CryptMap[bits >> 8];
	}
	*str = 0;

	return TRUE;
}

//=============================================================================
// MATH MODULE
//=============================================================================

#include "math.h"

//-----------------------------------------------------------------------------
// 8-bits fast 10 times division 
i8 Math_Div10(i8 val) __FASTCALL __PRESERVES(a, b, c, iyl, iyh)
{
	val; // L
	__asm
		ld		d, #0
		ld		h, d
		ld		e, l
		add		hl, hl
		add		hl, de
		add		hl, hl
		add		hl, hl
		add		hl, de
		add		hl, hl
		ld		l, h
	 __endasm;
}

//-----------------------------------------------------------------------------
// 18-bits fast modulo-10 
// Inputs:		A	8-bit unsigned integer
// Outputs:		A	HL mod 10
//				Z	Flag is set if divisible by 10
// 20 bytes, 83 cycles
u8 Math_Mod10(u8 val) __PRESERVES(b, c, d, e, iyl, iyh)
{
	val; // A
	__asm
		ld		h, a		// add nibbles
		rrca
		rrca
		rrca
		rrca
		add		a, h
		adc		a, #0		// n mod 15 (+1) in both nibbles
		daa
		ld		l, a
		sub		h			// test if quotient is even or odd
		rra
		sbc		a, a
		and		#5
		add		a, l
		daa
		and		#0x0F
	__endasm;
}

u16 g_RandomSeed8 = 1;

//-----------------------------------------------------------------------------
// Initialize random generator seed
void Math_SetRandomSeed8(u8 seed) { g_RandomSeed8 = (u16)seed; }

//-----------------------------------------------------------------------------
// Generates 8-bit pseudorandom numbers
// Ion Random by Joe Wingbermuehle (https://wikiti.brandonw.net/index.php?title=Z80_Routines:Math:Random)
u8 Math_GetRandom8()
{
	__asm
	#if(0)
		ld		hl, (_g_RandomSeed8)
		ld		a, r
		ld		d, a
		ld		e, (hl)
		add		hl, de
		add		a, l
		xor		h
		ld		(_g_RandomSeed8), hl
	#else // Alternative version (better distribution but a little bit slower)
		ld		hl, (_g_RandomSeed8)
		ld		a, r
		ld		d, a
		ld		e, a
		add		hl, de
		xor		l
		add		a
		xor		h
		ld		l, a
		ld		(_g_RandomSeed8), hl
	#endif
	__endasm;
}

//=============================================================================
// MEMORY MODULE
//=============================================================================

#include "memory.h"

//-----------------------------------------------------------------------------
// Fill a memory block with a given 16-bits value (minimal size of 2 bytes).
void Mem_Set_16b(u16 val, void* dest, u16 size)
{
	val;	// HL
	dest;	// DE
	size;	// SP[2]
	__asm
		push	de
		ex		de, hl
		ld		(hl), d
		inc		hl
		ld		(hl), e
		inc		hl
		ex		de, hl
		pop		hl
		// Get parameters
		pop		iy							// 16 cc (return address)
		pop		bc							// 11 cc (retreive size)
#if 0//(MEM_USE_VALIDATOR)
		// Skip if size < 2
		ld		a, b
		or		a, c
		jp		z, mem_fill16_end
		dec		bc
		jp		z, mem_fill16_end
		dec		bc
		jp		z, mem_fill16_end
#else
		dec		bc
		dec		bc
#endif
		// Do fill
		ldir								// 23/18 cc
	mem_fill16_end:
		jp		(iy)						// 10 cc
	__endasm;
}

