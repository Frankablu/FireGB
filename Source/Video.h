//
//  Video.h
//  GB Emulator
//
//  Created by Frank on 07/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef GB_Emulator_Video_h
#define GB_Emulator_Video_h

#include "main.h"

class Video
{
private:
    SDL_Surface *screen;
    
    SDL_Surface *vRam;
    unsigned char vRamData[8*384*4*8*8];
    
    SDL_Surface *vRamO1;
    unsigned char vRamDataO1[8*384*4*8*8];
    
    SDL_Surface *vRamO2;
    unsigned char vRamDataO2[8*384*4*8*8];
    
    SDL_Surface *tempFlip; //For performing Horizontal Flip & Vertical Flip
    
    SDL_Surface *backgroundScreen;
    SDL_Surface *windowScreen;
    SDL_Surface *spriteScreen;
    SDL_Surface *gameboyScreen;
	SDL_Surface *gameboyScreenScaled[9];
    
    int r[12], g[12], b[12];
    
    int LCDenabled;
    int windowTileMapSelect;
    int windowEnabled;
    int windowTileDataSelect;
    int backgroundTileMapSelect;
    int spriteSize;
    int spriteEnabled;
    int backgroundAndWindowEnabled;
    
    int backgroundScrollX;
    int backgroundScrollY;
    
    int windowPositionX;
    int windowPositionY;
    
    int spriteChange;
    
    void regenVRAM();

	int overSpeed;
	int speeding;

	unsigned long frameNumber;
	unsigned long startGameTime;
    
public:
    Video();
    void startScreen();
    void resizeScreen(int x, int y);
    
    void vSync();
    void hSync();
    
    unsigned char getLY();
    unsigned char getFF41();
    
    void informVideo(unsigned int address);
    void updateVRAM(unsigned int address);
    void updateBackground(unsigned int address);
    void updateWindow(unsigned int address);
    void updateSprites();
    
    int scale;
    int speedUp;
    int fullscreen;

	void resetFramerate();
};

#endif
