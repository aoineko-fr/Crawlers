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
echo  Convert Font...

REM %MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\font.h -pos 0 0 -size 8 8 -gap 0 0 -num 16 4 -trans 0xDED087 -bpc 1 -name g_DataFont -font 8 8 0x20 _

echo -----------------------------------------------------------------------------
echo  Convert SC2 Images...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\tiles.h -mode gm2 --gm2unique -name g_DataTiles -pos 0 0 -size 128 128

%MSXtk%\MSXimg.exe datasrc\menu.png -out content\select.h -mode gm2 -name g_DataSelect -pos 0 0 -size 256 192
%MSXtk%\MSXimg.exe datasrc\menu.png -out content\face1.h -mode gm2 -name g_DataFace1 -pos 0 216 -size 200 40 -offset 128
%MSXtk%\MSXimg.exe datasrc\menu.png -out content\face2.h -mode gm2 -name g_DataFace2 -pos 0 256 -size 200 40 -offset 128


REM %MSXtk%\MSXimg.exe datasrc\menu.png -out content\select.h -mode gm2 -name g_DataSelect -pos 0 0 -size 40 40 ^
	REM -l gm2 40 0 40 40 ^
	REM -l gm2 80 0 40 40 ^
	REM -l gm2 120 0 40 40 ^
	REM -l gm2 0 40 40 40 ^
	REM -l gm2 40 40 40 40 ^
	REM -l gm2 80 40 40 40 ^
	REM -l gm2 120 40 40 40

echo -----------------------------------------------------------------------------
echo  Convert sprite...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\sprites.h -pos 128 0 -size 8 8 -num 4 4 -name g_DataSprites -trans 0xDED087 -bpc 1
