fgb: Source/main.cpp Source/main.h Source/CPU.cpp Source/CPU.h Source/Memory.cpp Source/Memory.h Source/Video.cpp Source/Video.h Source/Sound.cpp Source/Sound.h Source/SDL_rotozoomALT.h
	c++ Source/main.cpp Source/CPU.cpp Source/Memory.cpp Source/Video.cpp Source/Sound.cpp Source/SDL_rotozoomALT.c -o Binary/fgblinux -lSDL -I/usr/include/SDL

install:
	cp Binary/fgblinux /usr/bin/fgb
	echo "Usage: fgb romName"
