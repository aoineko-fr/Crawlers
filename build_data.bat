@echo off
:: ____________________________
:: ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │
:: ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │
:: █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │
:: ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
::  Data cooking
::─────────────────────────────────────────────────────────────────────────────

:: Path
set Tools=..\..\tools
set MSXtk=%Tools%\MSXtk\bin

if not exist "content" ( md content )

echo -----------------------------------------------------------------------------
echo  Convert Music...

%MSXtk%\MSXbin.exe datasrc\sfx.akx -t g_DataSFX -ad -o content\sfx.h -at 0x0100
set ADDR=%errorlevel%

%Tools%\compress\Pletter\pletter.exe datasrc\music_intro.akg datasrc\music_intro.pl5
%MSXtk%\MSXbin.exe datasrc\music_intro.pl5 -t g_MusicIntro -ad -o content\music_intro.h -at %ADDR%
set ADDR=%errorlevel%

%Tools%\compress\Pletter\pletter.exe datasrc\music_game.akg datasrc\music_game.pl5
%MSXtk%\MSXbin.exe datasrc\music_game.pl5 -t g_MusicGame -ad -o content\music_game.h -at %ADDR%
set ADDR=%errorlevel%

%Tools%\compress\Pletter\pletter.exe datasrc\music_hurry.akg datasrc\music_hurry.pl5
%MSXtk%\MSXbin.exe datasrc\music_hurry.pl5 -t g_MusicHurry -ad -o content\music_hurry.h -at %ADDR%
set ADDR=%errorlevel%

%Tools%\compress\Pletter\pletter.exe datasrc\music_empty.akg datasrc\music_empty.pl5
%MSXtk%\MSXbin.exe datasrc\music_empty.pl5 -t g_MusicEmpty -ad -o content\music_empty.h -at %ADDR%

echo -----------------------------------------------------------------------------
echo  Convert Font...

REM %MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\font.h -pos 0 0 -size 8 8 -gap 0 0 -num 16 4 -trans 0xDED087 -bpc 1 -name g_DataFont -font 8 8 0x20 _

echo -----------------------------------------------------------------------------
echo  Convert SC2 Images...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\tiles.h -mode gm2 --gm2unique -compress pletter -name g_DataTiles -pos 0 0 -size 128 128 -at %errorlevel%

REM %MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\title.h -mode gm2 --gm2name 1 -name g_DataTitle -pos 0 0 -size 128 128 -l gm2 0 152 192 40

%MSXtk%\MSXimg.exe datasrc\menu.png -out content\face1.h -mode gm2 -compress pletter -name g_DataFace1 -pos 0 216 -size 40 40 -offset 128 -at %errorlevel% ^
	-l gm2 40  216 40 40 ^
	-l gm2 80  216 40 40 ^
	-l gm2 120 216 40 40

%MSXtk%\MSXimg.exe datasrc\menu.png -out content\face2.h -mode gm2 -compress pletter -name g_DataFace2 -pos 0 256 -size 40 40 -offset 128 -at %errorlevel% ^
	-l gm2 40  256 40 40 ^
	-l gm2 80  256 40 40 ^
	-l gm2 120 256 40 40

%MSXtk%\MSXimg.exe datasrc\menu.png -out content\select.h -mode gm2 -compress pletter -name g_DataSelect -pos 0 0 -size 56 24 -at %errorlevel% ^
	-l gm2 56  0 56 24 ^
	-l gm2 112 0 56 24 ^
	-l gm2 168 0 56 24 ^
	-l gm2 0   24 56 24 ^
	-l gm2 56  24 56 24 ^
	-l gm2 112 24 56 24 ^
	-l gm2 168 24 56 24 ^
	-l gm2 0   48 56 24 ^
	-l gm2 56  48 56 24 ^
	-l gm2 112 48 56 24 ^
	-l gm2 168 48 56 24 ^
	-l gm2 0   72 56 24 ^
	-l gm2 56  72 56 24 ^
	-l gm2 112 72 56 24 ^
	-l gm2 168 72 56 24 ^
	-l gm2 0   96 56 24 ^
	-l gm2 56  96 56 24 ^
	-l gm2 112 96 56 24 ^
	-l gm2 168 96 56 24 ^
	-l gm2 0   120 56 24 ^
	-l gm2 56  120 56 24 ^
	-l gm2 112 120 56 24 ^
	-l gm2 168 120 56 24 ^
	-l gm2 0   144 56 24 ^
	-l gm2 56  144 56 24 ^
	-l gm2 0   168 56 24 ^
	-l gm2 56  168 56 24 ^
	-l gm2 112 144 56 48 ^
	-l gm2 168 144 56 16 ^
	-l gm2 168 160 48 16

%MSXtk%\MSXimg.exe datasrc\logo.png -out content\logo_tile.h -mode gm2 -compress pletter -name g_DataLogoTile -pos 0 0 -size 48 16 -at %errorlevel% ^
	-l gm2 0 16 32 32

echo -----------------------------------------------------------------------------
echo  Convert sprite...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\sprites.h -mode sprt -compress pletter -name g_DataSprites -pos 128 0 -size 8 8 -num 8 4 -trans 0xDED087 -at %errorlevel%

%MSXtk%\MSXimg.exe datasrc\logo.png -out content\logo_sprt.h -mode sprt -compress pletter -name g_DataLogoSprt -pos 0 48 -size 32 48 -num 1 1 -at %errorlevel% ^
	-l i16 0  8 2 2 0xFF897D ^
	-l i16 0  0 2 1 0xB95E51 ^
	-l i16 0 16 2 1 0xFFFFFF ^
	-l i16 0 32 2 1 0xDED087