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
#include "content/sfx.h"
