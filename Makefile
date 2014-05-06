fgb: Source/main.cpp Source/main.h Source/CPU.cpp Source/CPU.h Source/Memory.cpp Source/Memory.h Source/Video.cpp Source/Video.h Source/Sound.cpp Source/Sound.h Source/Menu.cpp Source/Menu.h Source/SDL2_rotozoomALT.c Source/SDL2_rotozoomALT.h
	c++ Source/main.cpp Source/CPU.cpp Source/Memory.cpp Source/Video.cpp Source/Sound.cpp Source/Menu.cpp Source/SDL2_rotozoomALT.c -o Binary/fgblinux -lSDL2 -I/usr/include/SDL2 -lSDL2_ttf

install:
	cp Binary/fgblinux /usr/bin/fgb
	echo "Usage: fgb romName"
