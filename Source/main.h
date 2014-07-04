//
//  main.h
//  GB Emulator
//
//  Created by Frank on 06/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef GB_Emulator_main_h
#define GB_Emulator_main_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "SDLMain.h"
#include <SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_ttf.h>
#endif

#include "SDL2_rotozoomALT.h"
#include "Video.h"
#include "Memory.h"
#include "CPU.h"
#include "Sound.h"
#include "Menu.h"

#define KEYUP 4
#define KEYDOWN 8
#define KEYLEFT 2
#define KEYRIGHT 1
#define KEYA 1
#define KEYB 2
#define KEYSTART 8
#define KEYSELECT 4

#endif
