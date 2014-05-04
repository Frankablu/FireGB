//
//  VIDEO.cpp
//  GB Emulator
//
//  Created by Frank on 06/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Video.h"

//Screen buffer is 256x256
//SCROLL X SCROLL Y top left corner of a 160x144 displayable chunk
//The Video Display needs to Emulate a LCD display...

//Vsync is once every 16.6ms
//LY the vertical line to which present data is transferred to the LCD driver
//FF44 - 0-143 - Busy 144-153 Vsync

#ifndef VIDEO
#define VIDEO
extern CPU ourCPU;
extern long tick;
extern Memory ourMemory;

Video::Video()
{
    for (int i = 0; i < 4; i++)
    {
        r[i] = i*80; g[i] = i*80; b[i] = i*80; //Remains until gameboy cart sets colours
    }
    r[4] = 255; g[4] = 0; b[4] = 255; //Transparent
    r[8] = 255; g[8] = 0; b[8] = 255;
    
    LCDenabled = 1;
    spriteChange = 0;
}

void Video::resizeScreen(int x, int y)
{
    if (fullscreen)
    {
        screen = SDL_SetVideoMode(x, y, 16, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_DOUBLEBUF);
    }
    else
    {
        screen = SDL_SetVideoMode(x, y, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
    }
}

void Video::resetFramerate()
{
	frameNumber = 0;
	startGameTime = SDL_GetTicks();
}

void Video::startScreen()
{
    screen = SDL_SetVideoMode(160*4, 144*4, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
    vRam = SDL_CreateRGBSurfaceFrom(vRamData, 8, 384*4*8*8, 8*4, 4*8,0,0,0,0);
    vRamO1 = SDL_CreateRGBSurfaceFrom(vRamDataO1, 8, 384*4*8*8, 8*4, 4*8,0,0,0,0);
    vRamO2 = SDL_CreateRGBSurfaceFrom(vRamDataO2, 8, 384*4*8*8, 8*4, 4*8,0,0,0,0);
    backgroundScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 255*2, 255*2, 32, 0, 0, 0, 0);
    windowScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 255, 255, 32, 0, 0, 0, 0);
    spriteScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 255, 255, 32, 0, 0, 0, 0);
    tempFlip = SDL_CreateRGBSurface(SDL_HWSURFACE, 16, 16, 32, 0, 0, 0, 0);
    gameboyScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 160, 144, 32, 0, 0, 0, 0);
	
	for (int scaleS = 0; scaleS < 9; scaleS++)
	{
		gameboyScreenScaled[scaleS] = SDL_CreateRGBSurface(SDL_HWSURFACE, 160*scaleS, 144*scaleS, 32, 0, 0, 0, 0);
	}
    
    SDL_SetColorKey(spriteScreen, SDL_SRCCOLORKEY, SDL_MapRGB(spriteScreen->format, 255, 0, 255));
    SDL_SetColorKey(vRamO1, SDL_SRCCOLORKEY, SDL_MapRGB(vRamO1->format, 255, 0, 255));
    SDL_SetColorKey(vRamO2, SDL_SRCCOLORKEY, SDL_MapRGB(vRamO2->format, 255, 0, 255));
        
    windowPositionX = 0;
    windowPositionY = 0;
    scale = 4;
    speedUp = 0;
    fullscreen = 0;

	frameNumber = 0;
	startGameTime = SDL_GetTicks();
	overSpeed = 0;

	speeding = 0;
}

void Video::vSync()
{
	//Better Frame Rate Control
	if (!speedUp)
	{
		frameNumber++;
		long currentTick = SDL_GetTicks();
	
		while (currentTick < startGameTime + frameNumber*17) //Too fast!
		{
			SDL_Flip(screen);
			currentTick = SDL_GetTicks();
			overSpeed = 1;
		}
	
		if (!overSpeed) //Too slow
		{
			if (currentTick - startGameTime + frameNumber*17 > 17)
			{
				return; //Skip to catch up
			}
		}
	}

    //x5 Speed Mode - Spacebar
    if (speedUp)
    {
        speeding++;
        if (speeding == 5)
        {
            speeding = 0;
        }
        else
        {
            return;
        }
    }
    
    SDL_Rect src, dst;
    
    if (scale == 0) //Debug Screen
    {
        if (LCDenabled)
        {
        src.x = 0; src.y = 0;
        src.w = 8; src.h = 800;
        
        dst.x = 0; dst.y = 0;
        dst.w = 8; dst.h = 800;
        
        //Blit VRAM
        SDL_BlitSurface(vRam, &src, screen, &dst);
        src.y = 800;
        dst.x = 8;
        SDL_BlitSurface(vRam, &src, screen, &dst);
        
        //Blit Background VRAM
        src.x = 0; src.y = 0;
        src.w = 255; src.h = 255;
        dst.x = 40; dst.y = 0;
        dst.w = 255; dst.h = 255;
        SDL_BlitSurface(backgroundScreen,&src,screen,&dst);
        
        //Blit Window VRAM
        src.x = 0; src.y = 0;
        src.w = 255; src.h = 255;
        dst.x = 300; dst.y = 0;
        dst.w = 255; dst.h = 255;
        SDL_BlitSurface(windowScreen,&src,screen,&dst);
    }
        
    //Blit Sprite VRAM
    src.x = 0; src.y = 0;
    src.w = 255; src.h = 255;
    dst.x = 600; dst.y = 0;
    dst.w = 255; dst.h = 255;
        
    SDL_FillRect(screen, &dst, SDL_MapRGB(spriteScreen->format, 0, 0, 0));
    SDL_BlitSurface(spriteScreen,&src,screen,&dst);
        
    //Blit Gameboy Screen
    src.x = 0; src.y = 0;
    src.w = 160; src.h = 144;
    dst.x = 300; dst.y = 300;
    dst.w = 160; dst.h = 144;
    SDL_BlitSurface(gameboyScreen,&src,screen,&dst);
        
    }
    else
    {
        //Blit Scaled Gameboy Screen
        zoomSurface (gameboyScreen, gameboyScreenScaled[scale], scale, scale, 1);
        SDL_BlitSurface(gameboyScreenScaled[scale],NULL,screen,NULL);
    }
    
    //Effectively a VSync with the computer screen
	//std::cout << std::dec << "Current Tick! " << SDL_GetTicks() << std::endl;
    SDL_Flip(screen);
}

void Video::hSync()
{
    if (spriteChange)
    {
        updateSprites();
        spriteChange = 0;
    }
    
    int LY = getLY();
    SDL_Rect src, dst;
    
    if (LY <= 144)
    {
        if (backgroundAndWindowEnabled)
        {
            //Background with its scroll
            src.x = backgroundScrollX; src.y = backgroundScrollY + LY;
            src.w = 160; src.h = 1;
            
            dst.x = 0; dst.y = LY;
            dst.w = 160; dst.h = 1;
            
            SDL_BlitSurface(backgroundScreen, &src, gameboyScreen, &dst);
            
            if (windowEnabled && windowPositionY <= LY)
            {
                //Window with its position
                src.x = 0; src.y = LY - windowPositionY;
                src.w = 255; src.h = 1;
                
                dst.x = windowPositionX - 7; dst.y = LY;
                dst.w = 255; dst.h = 1;
                
                SDL_BlitSurface(windowScreen, &src, gameboyScreen, &dst);
            }
        }
        
        if (spriteEnabled)
        {
            src.x = 8; src.y = LY + 16;
            src.w = 160; src.h = 1;
            
            dst.x = 0; dst.y = LY;
            dst.w = 160; dst.h = 1;
            
            SDL_BlitSurface(spriteScreen, &src, gameboyScreen, &dst);
        }
    }
}

void Video::informVideo(unsigned int address)
{
    
    if (address >= 0x8000 && address < 0x9800)
    {
        updateVRAM(address);
        return;
    }
    
    if (address >=0x9800 && address <= 0x9FFF)
    {
        updateBackground(address);
        updateWindow(address);
        return;
    }
    
    if (address >=0xFE00 && address <= 0xFE9F)
    {
        //Sprites
        spriteChange = 1; //Just mark as changed will update before next HSync
        return;
    }
    
    unsigned char value = ourMemory.getValue2(address);
    
    int shade;
    
    switch (address)
    {
        case 0xFF40:
            LCDenabled = value>>7;
            windowTileMapSelect = (value>>6)&1;
            windowEnabled = (value>>5)&1;
            windowTileDataSelect = (value>>4)&1;
            backgroundTileMapSelect = (value>>3)&1;
            spriteSize = (value>>2)&1;
            spriteEnabled = (value>>1)&1;
            backgroundAndWindowEnabled = value&1;
            regenVRAM();
            break;
            
        case 0xFF42:
            backgroundScrollY = value;
            break;
            
        case 0xFF43:
            backgroundScrollX = value;
            break;
            
        //BG & Window Palette Data - BGP - BG Palette Data (R/W) - Non CGB Mode Only
        case 0xFF47:
            for (int place = 0; place < 4; place++)
            {
                shade = (value >> place * 2) & 0x3;
                switch (shade)
                {
                    case 0:
                        r[place] = 255; g[place] = 255; b[place] = 255;
                        break;
                        
                    case 1:
                        r[place] = 160; g[place] = 160; b[place] = 160;
                        break;
                        
                    case 2:
                        r[place] = 80; g[place] = 80; b[place] = 80;
                        break;
                        
                    case 3:
                        r[place] = 0; g[place] = 0; b[place] = 0;
                        break;
                }
            }
            regenVRAM();
            break;
            
        case 0xFF48: //OBP0 - Object Palette Data
            for (int place = 0; place < 4; place++)
            {
                shade = (value >> place * 2) & 0x3;
                switch (shade)
                {
                    case 0:
                        r[place+4] = 255; g[place+4] = 255; b[place+4] = 255;
                        break;
                        
                    case 1:
                        r[place+4] = 160; g[place+4] = 160; b[place+4] = 160;
                        break;
                        
                    case 2:
                        r[place+4] = 80; g[place+4] = 80; b[place+4] = 80;
                        break;
                        
                    case 3:
                        r[place+4] = 0; g[place+4] = 0; b[place+4] = 0;
                        break;
                }
            }
            r[4] = 255; g[4] = 0; b[4] = 255; //Transparent
            regenVRAM();
            break;
            
        case 0xFF49: //OBP1 - Object Palette Data
            for (int place = 0; place < 4; place++)
            {
                shade = (value >> place * 2) & 0x3;
                switch (shade)
                {
                    case 0:
                        r[place+8] = 255; g[place+8] = 255; b[place+8] = 255;
                        break;
                        
                    case 1:
                        r[place+8] = 160; g[place+8] = 160; b[place+8] = 160;
                        break;
                        
                    case 2:
                        r[place+8] = 80; g[place+8] = 80; b[place+8] = 80;
                        break;
                        
                    case 3:
                        r[place+8] = 0; g[place+8] = 0; b[place+8] = 0;
                        break;
                }
            }
            r[8] = 255; g[8] = 0; b[8] = 255; //Transparent
            regenVRAM();
            break;
        
        case 0xFF4A: //Window Y Position
            windowPositionY = value;
            break;
            
        case 0xFF4B: //Window X Position
            windowPositionX = value;
            break;
            
        default:
            break;
    }
}

void Video::updateVRAM(unsigned int address)
{
    //2 Bytes define the setting
    if (address % 2) { address--; }
    
    int place = address - 0x8000;
    int placeInTileData = place * 16;
    
    int currentPart;
    int first = ourMemory.getValue2(address);
    int second = ourMemory.getValue2(address + 1) << 1;
    
    for (int j = 7; j > -1; j--)
    {
        currentPart = (second&2) | (first&1);
        first = first >> 1;
        second = second >> 1;
        
        vRamData[placeInTileData+4*j] = r[currentPart];
        vRamData[placeInTileData+4*j+1] = g[currentPart];
        vRamData[placeInTileData+4*j+2] = b[currentPart];
        vRamData[placeInTileData+4*j+3] = 0;
        
        vRamDataO1[placeInTileData+4*j] = r[currentPart+4];
        vRamDataO1[placeInTileData+4*j+1] = g[currentPart+4];
        vRamDataO1[placeInTileData+4*j+2] = b[currentPart+4];
        vRamDataO1[placeInTileData+4*j+3] = 0;
        
        vRamDataO2[placeInTileData+4*j] = r[currentPart+8];
        vRamDataO2[placeInTileData+4*j+1] = g[currentPart+8];
        vRamDataO2[placeInTileData+4*j+2] = b[currentPart+8];
        vRamDataO2[placeInTileData+4*j+3] = 0;
    }
}

void Video::updateBackground(unsigned int address)
{
    int place = address;
    if (backgroundTileMapSelect)
    {
        place -= 0x9C00;
    }
    else
    {
        place -= 0x9800;
    }
    if (place < 0 || place > 1024) //Wrong Tile Map
    {
        return;
    }
    
    int placeX = place % 32;
    int placeY = place / 32;
    
    SDL_Rect src, dst;
    int tilePlace;
    
    if (windowTileDataSelect)
    {
        tilePlace = ourMemory.getValue2(address) * 8;
    }
    else
    {
        tilePlace = (int)(signed char)ourMemory.getValue2(address);
        tilePlace = tilePlace + 256;
        tilePlace = tilePlace * 8;
    }
    
    src.x = 0; src.y = tilePlace;
    src.w = 8; src.h = 8;
    
    dst.x = placeX*8; dst.y = placeY*8;
    dst.w = 8; dst.h = 8;
    
    SDL_BlitSurface(vRam, &src, backgroundScreen, &dst);
    dst.x += 255;
    SDL_BlitSurface(vRam, &src, backgroundScreen, &dst);
    dst.x -= 255; dst.y += 255;
    SDL_BlitSurface(vRam, &src, backgroundScreen, &dst);
    dst.x += 255;
    SDL_BlitSurface(vRam, &src, backgroundScreen, &dst);
}

void Video::updateWindow(unsigned int address)
{
    int place = address;
    if (windowTileMapSelect)
    {
        place -= 0x9C00;
    }
    else
    {
        place -= 0x9800;
    }
    if (place < 0 || place > 1024) //Wrong Tile Map
    {
        return;
    }
    
    int placeX = place % 32;
    int placeY = place / 32;
    
    SDL_Rect src, dst;
    int tilePlace;
    
    if (windowTileDataSelect)
    {
        tilePlace = ourMemory.getValue2(address) * 8;
    }
    else
    {
        tilePlace = (int)(signed char)ourMemory.getValue2(address);
        tilePlace = tilePlace + 256;
        tilePlace = tilePlace * 8;
    }
    
    src.x = 0; src.y = tilePlace;
    src.w = 8; src.h = 8;
    
    dst.x = placeX*8; dst.y = placeY*8;
    dst.w = 8; dst.h = 8;
    
    SDL_BlitSurface(vRam, &src, windowScreen, &dst);
}

void Video::regenVRAM()
{
    for (int address = 0x8000; address < 0x9800; address+=2)
    {
        updateVRAM(address);
    }
    for (int address = 0x9800; address <= 0x9FFF; address++)
    {
        updateBackground(address);
        updateWindow(address);
    }
}

void Video::updateSprites()
{
    SDL_Rect src, dst;
    dst.x = 0; dst.y = 0;
    dst.w = 255; dst.h = 255;
    SDL_FillRect(spriteScreen, &dst, SDL_MapRGB(spriteScreen->format, 255, 0, 255));
    
    int y,x,pattern,flags;
    for (int address = 0xFE00; address < 0xFE9F; address += 4)
    {
        y = ourMemory.getValue2(address);
        x = ourMemory.getValue2(address+1);
        pattern = ourMemory.getValue2(address+2);
        flags = ourMemory.getValue2(address+3);
        int vflip = (flags&64)>>6;
        int hflip = (flags&32)>>5;
        SDL_Surface *source;
        
        if (spriteSize)
        {
            pattern = pattern & 254; //Remove least significant bit if displaying size 16
        }
        
        //Sprite offset is -8, -16
        src.x = 0; src.y = pattern*8;
        src.w = 8; src.h = 8;
        
        dst.x = x; dst.y = y;
        dst.w = 8; dst.h = 8;
        
        if (spriteSize)
        {
            src.h = 16; dst.h = 16;
        }
        
        if (flags&16) //Colour Select
        {
            source = vRamO2;
        }
        else
        {
            source = vRamO1;
        }
        
        if (vflip == 0 && hflip == 0)
        {
            SDL_BlitSurface(source, &src, spriteScreen, &dst);
        }
        if (vflip && hflip == 0)
        {
            dst.h = 1; src.h = 1;
            src.y += 7 + (spriteSize * 8);
            for (int i = 0; i < 8 + (spriteSize * 8); i++)
            {
                SDL_BlitSurface(source, &src, spriteScreen, &dst);
                src.y--;
                dst.y++;
            }
        }
        if (hflip && vflip == 0)
        {
            dst.w = 1; src.w = 1;
            src.x += 7;
            for (int i = 0; i < 8; i++)
            {
                SDL_BlitSurface(source, &src, spriteScreen, &dst);
                src.x--;
                dst.x++;
            }
        }
        
        if (hflip && vflip)
        {
            dst.x = 0; dst.y = 0;
            
            //Clear temp buff before usage
            SDL_FillRect(tempFlip, &dst, SDL_MapRGB(spriteScreen->format, 255, 0, 255));
            
            //Vertical Flip to Temp Flip then Horizontal Flip To the screen
            
            dst.h = 1; src.h = 1;
            src.y += 7 + (spriteSize * 8);
            for (int i = 0; i < 8 + (spriteSize * 8); i++)
            {
                SDL_BlitSurface(source, &src, tempFlip, &dst);
                src.y--;
                dst.y++;
            }
            
            src.x = 0; src.y = 0;
            src.h = 8 + (spriteSize * 8);
            
            dst.x = x; dst.y = y;
            dst.h = 8 + (spriteSize * 8);
            
            dst.w = 1; src.w = 1;
            src.x += 7;
            for (int i = 0; i < 8; i++)
            {
                SDL_BlitSurface(tempFlip, &src, spriteScreen, &dst);
                src.x--;
                dst.x++;
            }
        }
        
    }
}

unsigned char Video::getLY()
{
    return (tick % 17556) / 114;
}

unsigned char Video::getFF41()
{
    //FF41 STAT Output
    int LY = getLY();
    int displayMode = 0;
    
    if (LY >= 144) //VSyncing
    {
        displayMode = 1;
    }
    else
    {
        //Determine what mode the video is in (Its a cycle)
        int period = (int)(tick % 113);
        displayMode = 0;
        if (period > 50)
        {
            displayMode = 2;
        }
        if (period > 71)
        {
            displayMode = 3;
        }
    }
    
    //Most of the information is just as set
    //But bits 1-0 contain the current video mode
    //Bit 2 is LYC coincidence
    unsigned char value = ourMemory.Mem[0xFF41];
    value = value & 248; //Keep Settings
    
    if (getLY() == ourMemory.getValue2(0xFF45))
    {
        value += 4;
    }
    
    value += displayMode;
    
    return value;
    
}

#endif
