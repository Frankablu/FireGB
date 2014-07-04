//
//  main.cpp
//  GB Emulator
//
//  Created by Frank on 05/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "main.h"

CPU ourCPU;
Video ourVideo;
Sound ourSound;
Memory ourMemory;
Menu ourMenu;

extern int audioMute;

long currentTick = 0;
long tick = 0;
int lastplace = 0;

#ifndef __APPLE__
#ifndef _WIN32
int main(int argc, char* argv[])
{
    SDL_main(argc, argv);
}
#endif
#endif

int SDL_main(int argc, char* argv[])
{
    SDL_Init( SDL_INIT_EVERYTHING );
    atexit(SDL_Quit);
    
    std::string romFileName;
    
    if (argc > 1)
    {
        romFileName = std::string(argv[1]);
    }
    
    std::cout << "FireGB Emulator v1.0" << std::endl;
    std::cout << "Usage fgb romName" << std::endl;
    std::cout << "Keys:" << std::endl << "Arrow Keys" << std::endl << "A: Z "<< std::endl << "B: X" << std::endl;
    std::cout << "Start: Enter, A" << std::endl << "Select: Backspace, S" << std::endl;
    std::cout << "Speed Mode: Space Bar" << std::endl;
    std::cout << "Fullscreen: F" << std::endl;
    std::cout << "Scale Screen: 1, 2, 3, 4, 5, 6, 7, 8" << std::endl;
    std::cout << "Mute: m" << std::endl;
    std::cout << "Have fun!" << std::endl;
    
    ourCPU.loadrom(romFileName);
    ourCPU.resetCPU();
    
    ourVideo.startScreen();
    
    std::string windowTitle;
    windowTitle.append("FireGB - ");
    windowTitle.append(romFileName);
    windowTitle.at(windowTitle.size()-3) = '\0';
    
    SDL_SetWindowTitle(ourVideo.screenwindow,windowTitle.c_str());
    
    int running = 1;
    
    ourMenu.load();
    
    while (running)
    {
        currentTick = SDL_GetTicks();
        
        if (ourMemory.ready) //Is a gameboy rom loaded?
        {
            ourCPU.cpucycle(); //Blocks until Gameboy VSync
            ourMemory.saveRAM(); //Create a save file
        }
        else
        {
            ourVideo.vSync(); //cpucycle calls this automatically when a rom is loaded
        }
            
        //Keyboard Input
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;
                
                case SDL_MOUSEBUTTONDOWN:
                    ourMenu.mouseClick((SDL_MouseButtonEvent*)&event);
                    break;
                    
                case SDL_KEYDOWN:
                    
                    switch( event.key.keysym.sym )
                {
                        
                    case SDLK_LEFT:
                        ourCPU.keyLeft = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&32) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_RIGHT:
                        ourCPU.keyRight = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&32) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_UP:
                        ourCPU.keyUp = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&32) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_DOWN:
                        ourCPU.keyDown = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&32) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_x:
                        ourCPU.keyB = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&16) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                    
                    case SDLK_z:
                        ourCPU.keyA = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&16) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_0:
                        ourVideo.scale = 0;
                        ourVideo.resizeScreen(1024, 768);
                        break;
                        
                    case SDLK_1:
                        ourVideo.scale = 1;
                        ourVideo.resizeScreen(160, 144);
                        break;
                        
                    case SDLK_2:
                        ourVideo.scale = 2;
                        ourVideo.resizeScreen(160*2, 144*2);
                        break;
                        
                    case SDLK_3:
                        ourVideo.scale = 3;
                        ourVideo.resizeScreen(160*3, 144*3);
                        break;
                        
                    case SDLK_4:
                        ourVideo.scale = 4;
                        ourVideo.resizeScreen(160*4, 144*4);
                        break;

					case SDLK_5:
                        ourVideo.scale = 5;
                        ourVideo.resizeScreen(160*5, 144*5);
                        break;
                        
                    case SDLK_6:
                        ourVideo.scale = 6;
                        ourVideo.resizeScreen(160*6, 144*6);
                        break;
                        
                    case SDLK_7:
                        ourVideo.scale = 7;
                        ourVideo.resizeScreen(160*7, 144*7);
                        break;
                        
                    case SDLK_8:
                        ourVideo.scale = 8;
                        ourVideo.resizeScreen(160*8, 144*8);
                        break;
                        
                    case SDLK_ESCAPE:
                        std::cout << "User quit" << std::endl;
                        exit(0);
                        break;
                        
                    case SDLK_f:
                        ourVideo.fullscreen = !ourVideo.fullscreen; //Toggle Fullscreen
                        ourVideo.scale = 4;
                        ourVideo.resizeScreen(160*4, 144*4);
                        break;
                        
                    case SDLK_m:
                        audioMute = !audioMute;
                        break;
                        
                    case SDLK_BACKSPACE:
                    case SDLK_RSHIFT:
                    case SDLK_s:
                        ourCPU.keySelect = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&16) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_a:
                    case SDLK_RETURN:
                        ourCPU.keyStart = 0;
                        ourCPU.stop = 0;
                        if ((ourCPU.keyboardselect&16) != 0) {ourCPU.keyInterrupt = 1;}
                        break;
                        
                    case SDLK_SPACE:
                        ourVideo.speedUp = 1;
                        break;
                        
                    default:
                        break;
                }
                    break;
                    
                case SDL_KEYUP:
                    
                    switch( event.key.keysym.sym )
                {
                    case SDLK_LEFT:
                        ourCPU.keyLeft = 1;
                        break;
                        
                    case SDLK_RIGHT:
                        ourCPU.keyRight = 1;
                        break;
                        
                    case SDLK_UP:
                        ourCPU.keyUp = 1;
                        break;
                        
                    case SDLK_DOWN:
                        ourCPU.keyDown = 1;
                        break;
                        
                    case SDLK_x:
                        ourCPU.keyB = 1;
                        break;
                        
                    case SDLK_z:
                        ourCPU.keyA = 1;
                        break;
                        
                    case SDLK_s:
                    case SDLK_BACKSPACE:
                    case SDLK_RSHIFT:
                        ourCPU.keySelect = 1;
                        break;
                        
                    case SDLK_a:
                    case SDLK_RETURN:
                        ourCPU.keyStart = 1;
                        break;
                        
                    case SDLK_SPACE:
                        ourVideo.speedUp = 0;
						ourVideo.resetFramerate();
                        break;
                        
                    default:
                        break;
                }
                    break;
            }
        }
        
    }
    
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}

