//
//  CPU.h
//  GB Emulator
//
//  Created by Frank on 07/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef GB_Emulator_CPU_h
#define GB_Emulator_CPU_h

#include <iostream>

class CPU
{
public:
    unsigned short PC; //Program Counter
    unsigned short SP; //Stack Pointer
    
    int reading;
    
    //CPU Registers
    unsigned char A;
    unsigned char F;
    
    unsigned char B;
    unsigned char C;
    
    unsigned char D;
    unsigned char E;
    
    unsigned char H;
    unsigned char L;
    
    int halt;
    int stop;
    int interrupts;
    
    //Keyboard
    unsigned char keyboardselect;
    int keyboardUp;
    
    int keyUp, keyDown, keyLeft, keyRight, keyA, keyB, keyStart, keySelect;
    
    //Clocks
    long clocktime;
    long clocktime2;
    long timerOverflow;
    long divider;
    long tima;
    
    long nextVsync;
    
    void loadrom(std::string romname);
    
    //CPU Functions
    void resetCPU();
    void cpucycle();
    inline int cpuinterrupts();
    
    inline unsigned char d8();
    inline unsigned short d16();
    inline unsigned char a8();
    inline unsigned char a16();
    inline signed char r8();
    inline void setA8(unsigned char value);
    inline void setA16(unsigned char value);
    
    inline unsigned short AF();
    inline unsigned short BC();
    inline unsigned short DE();
    inline unsigned short HL();
    
    inline void setAF(unsigned short value);
    inline void setBC(unsigned short value);
    inline void setDE(unsigned short value);
    inline void setHL(unsigned short value);
    
    inline void Flag(int Z, int N, int H, int C);
    
    inline void push(unsigned short value, char type);
    inline unsigned short pop(char type);
    
    inline void addA(unsigned char value);
    inline void adcA(unsigned char value);
    inline void subA(unsigned char value);
    inline void sbcA(unsigned char value);
    inline void cpA(unsigned char value);
    
    inline unsigned char ZF();
    inline unsigned char SF();
    inline unsigned char HF();
    inline unsigned char CF();
    
    //I/O Register Intercept
    unsigned char getValue(unsigned short address);
    void setValue(unsigned int address, unsigned char value);
    
    int vsynced;
    int vsyncRequest;
    
    int hsynced;
    int timerSpeed;
    int timerPlace;
    int timerPlaceB;
    
    long hsyncTime;
    long vsyncTime;
    long lastTimerUpdate;
    int timeAccumulator;
    
    long nextInterrupt;
    int keyInterrupt;
    
    inline int cpuInterrupts();
    
};

#endif
