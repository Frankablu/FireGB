//
//  Menu.h
//  FireGBXCode
//
//  Created by Frank Burton on 05/05/2014.
//
//

#ifndef __FireGBXCode__Menu__
#define __FireGBXCode__Menu__

#include <iostream>
#include "main.h"

class Menu
{
private:
    TTF_Font *font;
    int menuState;
    std::string workingDirectory;
    int scroll;
    std::vector<std::string*> fileList;
    std::string lastDirectory;
    SDL_Surface *pMenuOptions[30];
public:
    void load();
    void displayMenu(SDL_Surface *screen);
    void mouseClick(SDL_MouseButtonEvent *mouse);
    std::string getDirectory();
    std::vector<std::string*> getFileList(std::string directory);
	std::vector<std::string*> getFileListDirOnly(std::string directory);
};


#endif

