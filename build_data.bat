@echo off
:: ____________________________
:: ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │
:: ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │
:: █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │
:: ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
::  Data cooking
::─────────────────────────────────────────────────────────────────────────────

cd .\datasrc

echo -----------------------------------------------------------------------------
echo  Convert SC2 Images...

..\..\..\tools\MSXtk\bin\MSXimg.exe crawlers.png -out ..\content\tiles.h -mode gm2 --gm2unique -name g_DataTiles -pos 0 0 -size 128 128

REM pause
cd ..
