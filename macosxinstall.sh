#!/bin/bash
echo "FGB Mac OS X Install Script"
echo "(You may need to insert your root password here)"
sudo cp Binary/fgbmacosx /usr/bin/fgb
sudo cp ExtraFiles/LiberationSerif-Regular.ttf /usr/bin/

echo "You may need to install SDL 2.0 and SDL_ttf before usage"
echo "Type fgb romName to run"
