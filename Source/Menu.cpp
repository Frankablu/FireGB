//
//  Menu.cpp
//  FireGBXCode
//
//  Created by Frank Burton on 05/05/2014.
//
//

#include "Menu.h"
extern Video ourVideo;
extern int audioMute;
extern CPU ourCPU;
extern Sound ourSound;

void Menu::load()
{
    TTF_Init();
    font = TTF_OpenFont("LiberationSerif-Regular.ttf", 18);
	if (!font)
	{
		std::cout << "File missing LiberationSerif-Regular.ttf" << std::endl;
		exit(1);
	}
    menuState = 0;
    workingDirectory = getDirectory();
    scroll = 0;
    lastDirectory = "";
    
    SDL_Color black;
    black.r = 0;
    black.g = 0;
    black.b = 0;
    
    pMenuOptions[0] = TTF_RenderText_Blended(font, "File", black);
    pMenuOptions[1] = TTF_RenderText_Blended(font, "Video", black);
    pMenuOptions[2] = TTF_RenderText_Blended(font, "Sound", black);
    pMenuOptions[3] = TTF_RenderText_Blended(font, "Controls", black);
    pMenuOptions[4] = TTF_RenderText_Blended(font, "Hide Menu", black);
    pMenuOptions[5] = TTF_RenderText_Blended(font, "Load ROM", black);
    pMenuOptions[6] = TTF_RenderText_Blended(font, "Exit", black);
    pMenuOptions[7] = TTF_RenderText_Blended(font, "Fullscreen", black);
    pMenuOptions[8] = TTF_RenderText_Blended(font, "Scale x1", black);
    pMenuOptions[9] = TTF_RenderText_Blended(font, "Scale x2", black);
    pMenuOptions[10] = TTF_RenderText_Blended(font, "Scale x4", black);
    pMenuOptions[11] = TTF_RenderText_Blended(font, "Scale x8", black);
    pMenuOptions[12] = TTF_RenderText_Blended(font, "View Internals", black);
    pMenuOptions[13] = TTF_RenderText_Blended(font, "Turn Sound On", black);
    pMenuOptions[14] = TTF_RenderText_Blended(font, "Turn Sound Off", black);
    pMenuOptions[15] = TTF_RenderText_Blended(font, "Left - Left Arrow", black);
    pMenuOptions[16] = TTF_RenderText_Blended(font, "Right - Right Arrow", black);
    pMenuOptions[17] = TTF_RenderText_Blended(font, "Up - Up Arrow", black);
    pMenuOptions[18] = TTF_RenderText_Blended(font, "Down - Down Arrow", black);
    pMenuOptions[19] = TTF_RenderText_Blended(font, "A - Z", black);
    pMenuOptions[20] = TTF_RenderText_Blended(font, "B - X", black);
    pMenuOptions[21] = TTF_RenderText_Blended(font, "Start - Enter", black);
    pMenuOptions[22] = TTF_RenderText_Blended(font, "Select - Right Shift", black);
    pMenuOptions[23] = TTF_RenderText_Blended(font, "Scroll Down", black);
    pMenuOptions[24] = TTF_RenderText_Blended(font, "Scroll Up", black);
	
}

void Menu::displayMenu(SDL_Surface *screen)
{
	
    SDL_Rect src,dst;
    SDL_Color black;
    black.r = 0;
    black.g = 0;
    black.b = 0;
    dst.x = 10; dst.y = 5;
    
    if (ourVideo.scale < 4) //Don't display menu on small screens
    {
        return;
    }
    
    SDL_Surface *menuOptions;
    
    if (menuState != -1)
    {
        src.x = 0; src.y = 0; src.h = 30; src.w = 640;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        SDL_BlitSurface (pMenuOptions[0],NULL,screen,&dst);
        dst.x += 100;
        
        SDL_BlitSurface (pMenuOptions[1],NULL,screen,&dst);
        dst.x += 100;
        
        SDL_BlitSurface (pMenuOptions[2],NULL,screen,&dst);
        dst.x += 100;
        
        SDL_BlitSurface (pMenuOptions[3],NULL,screen,&dst);
        dst.x += 100;
        
        SDL_BlitSurface (pMenuOptions[4],NULL,screen,&dst);
    }
    
    if (menuState == 1)
    {
        src.x = 0; src.y = 24; src.h = 10+24*2; src.w = 100;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        dst.x = 10;
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[5],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[6],NULL,screen,&dst);

    }
    
    if (menuState == 2)
    {
        src.x = 100; src.y = 24; src.h = 10+24*6; src.w = 130;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        dst.x = 110;
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[7],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[8],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[9],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[10],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[11],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[12],NULL,screen,&dst);
        dst.y += 24;
        
    }
    
    if (menuState == 3)
    {
        src.x = 200; src.y = 24; src.h = 10+24*1; src.w = 160;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        dst.x = 210;
        dst.y += 24;
        
        if (audioMute)
        {
            menuOptions = pMenuOptions[13];
        }
        else
        {
            menuOptions = pMenuOptions[14];
        }
        SDL_BlitSurface (menuOptions,NULL,screen,&dst);
        dst.y += 24;
        
    }
    
    if (menuState == 4)
    {
        src.x = 300; src.y = 24; src.h = 10+24*8; src.w = 200;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        dst.x = 310;
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[15],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[16],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[17],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[18],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[19],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[20],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[21],NULL,screen,&dst);
        dst.y += 24;
        
        SDL_BlitSurface (pMenuOptions[22],NULL,screen,&dst);
        dst.y += 24;
        
    }
    
    if (menuState == 5)
    {
        src.x = 40; src.y = 40; src.h = 500; src.w = 550;
        SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 200, 200, 200));
        
        dst.x = 50; dst.y = 50;
        
        menuOptions = TTF_RenderText_Blended(font, workingDirectory.c_str(), black);
        SDL_BlitSurface (menuOptions,NULL,screen,&dst);
        SDL_FreeSurface(menuOptions);
        dst.y += 24;
        
        menuOptions = TTF_RenderText_Blended(font, "..", black);
        SDL_BlitSurface (menuOptions,NULL,screen,&dst);
        SDL_FreeSurface(menuOptions);
        dst.y += 24;
        
        std::vector<std::string*> fileList = getFileList(workingDirectory);
        for (int a = 0; a + scroll < fileList.size() && a < 17; a++)
        {
            menuOptions = TTF_RenderText_Blended(font, fileList.at(a+scroll)->c_str(), black);
            SDL_BlitSurface (menuOptions,NULL,screen,&dst);
            SDL_FreeSurface(menuOptions);
            dst.y += 24;
        }
        
        dst.y = 50+24*19;
        SDL_BlitSurface (pMenuOptions[23],NULL,screen,&dst);
        dst.x += 300;
        
        SDL_BlitSurface (pMenuOptions[24],NULL,screen,&dst);
        dst.x += 300;
        
    }
	
}

void Menu::mouseClick(SDL_MouseButtonEvent *mouse)
{
    if (mouse->button == SDL_BUTTON_LEFT)
    {
        int x, y;
        SDL_GetMouseState(&x,&y);
        
        int screenW, screenH;
        SDL_GetWindowSize(ourVideo.screenwindow, &screenW, &screenH);
        double screenMultiplyer = (144.0*4.0) / (double)screenH;
        
        //Handle letter box
        int screenDifference = ((double)screenW - ((double)screenH * 1.11111)) / 2.0;
        x-= screenDifference;
        
        x = x * screenMultiplyer;
        y = y * screenMultiplyer;
        
        bool clickHandled = false;
        
        //Load ROM Button
        if (menuState == 1 && x > 10 && x < 110 && y > 34 && y < 34 + 24)
        {
            clickHandled = true;
            menuState = 5;
        }
        
        //Exit Button
        if (menuState == 1 && x > 10 && x < 110 && y > 34 + 24 && y < 34 + 48)
        {
            std::cout << "User quit" << std::endl;
            exit(0);
        }
        
        //Fullscreen Button
        if (menuState == 2 && x > 110 && x < 210 && y > 34 && y < 34 + 24)
        {
            clickHandled = true;
            ourVideo.fullscreen = !ourVideo.fullscreen; //Toggle Fullscreen
            ourVideo.scale = 4;
            ourVideo.resizeScreen(160*4, 144*4);
        }
        
        //Scale x1 Button
        if (menuState == 2 && x > 110 && x < 210 && y > 34 + 24 && y < 34 + 48)
        {
            clickHandled = true;
            ourVideo.scale = 1;
            ourVideo.resizeScreen(160*1, 144*1);
        }
        
        //Scale x2 Button
        if (menuState == 2 && x > 110 && x < 210 && y > 34 + 48 && y < 34 + 72)
        {
            clickHandled = true;
            ourVideo.scale = 2;
            ourVideo.resizeScreen(160*2, 144*2);
        }
        
        //Scale x4 Button
        if (menuState == 2 && x > 110 && x < 210 && y > 34 + 72 && y < 34 + 24 + 96)
        {
            clickHandled = true;
            ourVideo.scale = 4;
            ourVideo.resizeScreen(160*4, 144*4);
        }
        
        //Scale x8 Button
        if (menuState == 2 && x > 110 && x < 210 && y > 34 + 96 && y < 34 + 24 + 120)
        {
            clickHandled = true;
            ourVideo.scale = 8;
            ourVideo.resizeScreen(160*8, 144*8);
        }
        
        //Mute Button
        if (menuState == 3 && x > 210 && x < 310 && y > 34 && y < 34 + 24)
        {
            clickHandled = true;
            audioMute = !audioMute;
        }
        
        //Advanced View
        if (menuState == 2 && x > 110 && x < 210 && y > 34 + 120 && y < 34 + 24 + 144)
        {
            clickHandled = true;
            ourVideo.scale = 0;
            ourVideo.resizeScreen(1024,800);
        }
        
        //Rom Load
        if (menuState == 5 && x > 50 && y > 50 && x < 550 && y < 506)
        {
            //std::cout << "Load Rom Click" << std::endl;
            int clickEntry = (y - 50) / 24.0;
            std::vector<std::string*> fileList = getFileList(workingDirectory);
            
            if (clickEntry == 1)
            {
#ifndef _WIN32
                if (workingDirectory.find_last_of("/") != std::string::npos)
                {
                    workingDirectory = workingDirectory.substr(0,workingDirectory.find_last_of("/"));
                    scroll = 0;
                }
                if (workingDirectory == "")
                {
                    workingDirectory = "/";
                }
#else
				if (workingDirectory.find_last_of("\\") != std::string::npos)
                {
                    workingDirectory = workingDirectory.substr(0,workingDirectory.find_last_of("\\"));
                    scroll = 0;
                }
				if (workingDirectory.size() == 2)
				{
					workingDirectory.push_back('\\');
				}
#endif

                //std::cout << workingDirectory << std::endl;
            }
            
            if (clickEntry >= 2 && clickEntry < fileList.size() + 2)
            {
                std::string fullPath;
                fullPath.append(workingDirectory);
                fullPath.append("/");
                fullPath.append(*fileList[clickEntry-2+scroll]);
                
				std::string filename;
				filename.append(*fileList[clickEntry-2+scroll]);

                //std::cout << std::dec << "Clicked Entry: " << fullPath << std::endl;
                int isDirectory = 0;
#ifndef _WIN32
	isDirectory = (fullPath.at(fullPath.size()-1)=='/');
#else
	isDirectory = 0;

	std::vector<std::string*> fileList = getFileListDirOnly(workingDirectory);
	for (int i = 0; i < fileList.size(); i++)
	{
		if (*fileList.at(i) == filename)
		{
			isDirectory = 1;
		}
	}
#endif
                
	if (isDirectory)
	{
					#ifndef _WIN32
					if (workingDirectory.at(workingDirectory.size() - 1) != '/')
					{
						workingDirectory.append("/");
					}
					#else
					if (workingDirectory.at(workingDirectory.size() - 1) != '\\')
					{
						workingDirectory.append("\\");
					}
					#endif
                    workingDirectory.append(filename);
                    if (workingDirectory.at(workingDirectory.size()-1) == '/')
                    {
                        workingDirectory.resize(workingDirectory.size()-1);
                    }
                }
                else
                {
                    //std::cout << "Path: " << fullPath << std::endl;
                    if (fullPath.at(fullPath.size()-1) == '*')
                    {
                        fullPath.at(fullPath.size()-1) = '\0';
                    }
                    ourCPU.loadrom(fullPath);
                    ourCPU.resetCPU();
                    
                    std::string windowTitle;
                    windowTitle.append("FireGB - ");
                    windowTitle.append(filename);
                    windowTitle.at(windowTitle.size()-3) = '\0';
                    
                    SDL_SetWindowTitle(ourVideo.screenwindow,windowTitle.c_str());
                    
                    menuState = -1;
                }
                
            }
            
            clickHandled = true;
        }
        if (menuState == 5 && x > 50 && y >= 506 && x < 350 && y < 530) //Scroll Down
        {
            clickHandled = true;
            scroll+=10;
        }
        if (menuState == 5 && x >= 350 && y >= 506 && x < 560 && y < 530) //Scroll Up
        {
            clickHandled = true;
            scroll-=10;
            if (scroll < 0)
            {
                scroll = 0;
            }
        }
        
        
        //Menu Select
        if (x > 10 && x < 110 && y < 34)
        {
            clickHandled = true;
            menuState = 1;
        }
        
        if (x > 110 && x < 210 && y < 34)
        {
            clickHandled = true;
            menuState = 2;
        }
        
        if (x > 210 && x < 310 && y < 34)
        {
            clickHandled = true;
            menuState = 3;
        }
        
        if (x > 310 && x < 410 && y < 34)
        {
            clickHandled = true;
            menuState = 4;
        }
        
        if (x > 410 && x < 510 && y < 34)
        {
            clickHandled = true;
            menuState = -1;
        }
        
        if (clickHandled == false)
        {
            if (menuState != 0)
            {
                menuState = 0;
                if (ourVideo.scale < 4)
                {
                    ourVideo.scale = 4;
                }
            }
            else
            {
                menuState = -1;
            }
        }
        
    }
}

std::string Menu::getDirectory()
{
#ifndef _WIN32
    system("pwd > fgbtemp");
#else
	system("echo %CD% > fgbtemp");
#endif
    std::string line;
    std::ifstream myfile ("fgbtemp");
    if (myfile.is_open())
    {
        getline(myfile, line);
        myfile.close();
    }
#ifdef _WIN32
	line = line.substr(0,line.size()-1);
#endif
    return line;
}


std::vector<std::string*> Menu::getFileList(std::string directory)
{
    if (lastDirectory == directory)
    {
        return fileList;
    }
    
    for (int i = 0; i < fileList.size(); i++)
    {
        delete fileList.at(i);
    }
    fileList.clear();
    
    std::string commandString;
#ifndef _WIN32
    commandString.append("ls -F \"");
	commandString.append(directory);
    commandString.append("\" > fgbtemp");
#else
	commandString.append("dir /B \"");
	commandString.append(directory);
    commandString.append("\" > fgbtemp");
#endif
    
    system(commandString.c_str());

	std::string *line = new std::string();
    std::ifstream myfile ("fgbtemp");
    if (myfile.is_open())
    {
        while ( getline (myfile,*line) )
        {
            fileList.push_back(line);
            line = new std::string();
        }
        myfile.close();
    }
    
    lastDirectory = directory;
    return fileList;
}

std::vector<std::string*> Menu::getFileListDirOnly(std::string directory)
{
	lastDirectory = "!";

    for (int i = 0; i < fileList.size(); i++)
    {
        delete fileList.at(i);
    }
    fileList.clear();
    
    std::string commandString;
#ifndef _WIN32
    commandString.append("ls -F \"");
	commandString.append(directory);
    commandString.append("\" > fgbtemp");
#else
	commandString.append("dir /B /AD \"");
	commandString.append(directory);
    commandString.append("\" > fgbtemp");
#endif
    
    system(commandString.c_str());

	std::string *line = new std::string();
    std::ifstream myfile ("fgbtemp");
    if (myfile.is_open())
    {
        while ( getline (myfile,*line) )
        {
            fileList.push_back(line);
            line = new std::string();
        }
        myfile.close();
    }
    
    lastDirectory = directory;
    return fileList;
}

