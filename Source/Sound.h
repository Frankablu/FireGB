//
//  Sound.h
//  GB Emulator
//
//  Created by Frank on 07/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef GB_Emulator_Sound_h
#define GB_Emulator_Sound_h

#include <cmath>

class Sound
{
public:
    void informSound(unsigned int address, unsigned int value);
    unsigned char requestValue(unsigned int address, unsigned int lastValue);
    void startSound();
    void updateSound();
private:
    void evaluateSoundLevels();
};


#endif
