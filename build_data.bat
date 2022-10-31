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

echo -----------------------------------------------------------------------------
echo  Convert SC2 Images...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\tiles.h -mode gm2 --gm2unique -name g_DataTiles -pos 0 0 -size 128 128

echo -----------------------------------------------------------------------------
echo  Convert sprite...

%MSXtk%\MSXimg.exe datasrc\crawlers.png -out content\sprites.h -pos 128 0 -size 8 8 -num 4 2 -name g_DataSprites -trans 0xDED087 -bpc 1
