@echo off
rem Builds the normal ROM (hookedsmith.gba) and the debug ROM (hookedsmith_debug.gba).
rem The debug build uses its own build_debug folder so the two never mix.
rem Linking fails if mGBA has the ROM open. Keep this file ASCII only (cmd reads it as the system code page).
set DEVKITARM=C:\devkitPro\devkitARM
set DEVKITPRO=C:\devkitPro
set MAKEEXE=C:\devkitPro\msys2\usr\bin\make.exe
cd /d "%~dp0.."

echo === normal build ===
"%MAKEEXE%" -j8 || exit /b 1

echo === debug build ===
"%MAKEEXE%" -j8 TARGET=hookedsmith_debug BUILD=build_debug USERFLAGS=-DHS_DEBUG || exit /b 1

echo.
echo done: hookedsmith.gba (normal), hookedsmith_debug.gba (debug)
