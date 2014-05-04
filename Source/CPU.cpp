//
//  CPU.cpp
//  GB Emulator
//
//  Created by Frank on 05/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "main.h"
#include <stdio.h>

//This file is where we define our processer for our gameboy emulator

#ifndef GBCPU
#define GBCPU

#define FlagZ 128
#define FlagN 64
#define FlagH 32
#define FlagC 16

extern Video ourVideo;
extern Sound ourSound;
extern Memory ourMemory;

extern long tick;
int LYHack = 0;

void CPU::loadrom(std::string romname)
{
    ourMemory.loadrom(romname);
}

void CPU::resetCPU()
{
    PC = 0x100;
    halt = 0;
    stop = 0;
    interrupts = 1;
    
    keyboardselect = 0;
    tick = 0;
    clocktime = 0;
    clocktime2 = 0;
    timerOverflow = 0;
    nextVsync = 0;
    tima = 0;
    divider = 0;
    reading = 0;
    vsynced = 0;
    ourMemory.dmaTransfer = -1;
    vsyncRequest = 0;
    hsyncTime = 0;
    vsyncTime = 16416;
    lastTimerUpdate = 0;
    timeAccumulator = 0;
    keyInterrupt = 0;
    
    A = 0x01;
    F = 0xB0;
    B = 0; C = 0x13;
    D = 0; E = 0xD8;
    H = 0x01; L = 0x4D;
    SP = 0xFFFE;
    
    ourMemory.setValue(0xFF05, 0x00);
    ourMemory.setValue(0xFF06, 0x00);
    ourMemory.setValue(0xFF07, 0x00);
    ourMemory.setValue(0xFF10, 0x80);
    ourMemory.setValue(0xFF11, 0xBF);
    ourMemory.setValue(0xFF12, 0xF3);
    ourMemory.setValue(0xFF14, 0xBF);
    ourMemory.setValue(0xFF16, 0x3F);
    ourMemory.setValue(0xFF17, 0x00);
    ourMemory.setValue(0xFF19, 0xBF);
    ourMemory.setValue(0xFF1A, 0x7F);
    ourMemory.setValue(0xFF1B, 0xFF);
    ourMemory.setValue(0xFF1C, 0x9F);
    ourMemory.setValue(0xFF1E, 0xBF);
    ourMemory.setValue(0xFF20, 0xFF);
    ourMemory.setValue(0xFF21, 0x00);
    ourMemory.setValue(0xFF22, 0x00);
    ourMemory.setValue(0xFF23, 0xBF);
    ourMemory.setValue(0xFF24, 0x77);
    ourMemory.setValue(0xFF25, 0xF3);
    ourMemory.setValue(0xFF26, 0xF1);
    ourMemory.setValue(0xFF40, 0x91);
    ourMemory.setValue(0xFF42, 0x00);
    ourMemory.setValue(0xFF43, 0x00);
    ourMemory.setValue(0xFF45, 0x00);
    ourMemory.setValue(0xFF47, 0xFC);
    ourMemory.setValue(0xFF48, 0xFF);
    ourMemory.setValue(0xFF49, 0xFF);
    ourMemory.setValue(0xFF4A, 0x00);
    ourMemory.setValue(0xFF4B, 0x00);
    ourMemory.setValue(0xFFFF, 0x00);
    
    //Clear Keyboard
    keyUp = keyDown = keyLeft = keyRight = keyA = keyB = keyStart = keySelect = 1;
    keyboardUp = 0;
    
    timerPlaceB = 16;
    
    //cpuTest.debugInit(A, B, C, D, E, F, H, L, PC, SP);
    
    //Switch To Other Emulator Settings
    //A = 0x11; F = 0x90; //Changes gameboy type! - Incorrectly Sets Flag registers!
    
    nextInterrupt = -1;
}

//Intercept calls to the registers
unsigned char CPU::getValue(unsigned short address)
{
    return ourMemory.getValue(address);
}

void CPU::setValue(unsigned int address, unsigned char value)
{
    ourMemory.setValue(address, value);
}

inline unsigned char CPU::d8()
{
    unsigned char value = getValue(PC);
    PC++;
    return value;
}

inline unsigned short CPU::d16()
{
    unsigned short value = getValue(PC);
    value |= (getValue(PC+1) << 8);
    PC+=2;
    return value;
}

inline unsigned char CPU::a8()
{
    unsigned char value = getValue((0xFF00 + getValue(PC)));
    PC++;
    return value;
}

inline void CPU::setA8(unsigned char value)
{
    setValue((0xFF00|getValue(PC)), value);
    PC++;
}

inline unsigned char CPU::a16()
{
    unsigned short value;
    value = getValue(PC) | (getValue(PC+1) << 8);
    unsigned char value2 = getValue(value);
    PC+=2;
    return value2;
}

inline void CPU::setA16(unsigned char value)
{
    unsigned short address;
    address = getValue(PC);
    address |= (getValue(PC+1) << 8);
    PC+=2;
    setValue(address, value);
}

inline signed char CPU::r8()
{
    unsigned char value;
    value = getValue(PC);
	
	signed char valueb = value;
    PC++;
    return valueb;
}

inline unsigned short CPU::AF()
{
    return ((((unsigned short)A) << 8) | (F&0xF0));
}

inline unsigned short CPU::BC()
{
    return ((((unsigned short)B) << 8) | C);
}

inline unsigned short CPU::DE()
{
    return ((((unsigned short)D) << 8) | E);
}

inline unsigned short CPU::HL()
{
    return ((((unsigned short)H) << 8) | L);
}

inline void CPU::setAF(unsigned short value)
{
    A = (value >> 8);
    F = (value&0xF0);
}

inline void CPU::setBC(unsigned short value)
{
    B = (value >> 8);
    C = value;
}

inline void CPU::setDE(unsigned short value)
{
    D = (value >> 8);
    E = value;
}

inline void CPU::setHL(unsigned short value)
{
    H = (value >> 8);
    L = value;
}

inline unsigned char CPU::ZF() //Zero Flag?
{
    if ((F&128)!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

inline unsigned char CPU::SF() //Subtract Flag?
{
    if ((F&64)!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
inline unsigned char CPU::HF() //Half Carry Flag?
{
    if ((F&32)!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

inline unsigned char CPU::CF() //Carry Flag?
{
    if ((F&16)!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void CPU::push(unsigned short value, char status)
{
    SP--;
    setValue(SP, value >> 8);
    SP--;
    setValue(SP, value);
}

inline unsigned short CPU::pop(char status)
{
    unsigned short value;
    value = getValue(SP);
    SP++;
    value = value | getValue(SP) << 8;
    SP++;
    
    return value;
}

inline void CPU::addA(unsigned char value)
{
    //The complex flags
    Flag(0,0,((A&0xF)+(value&0xF)>0xF),(A+value>255));
    
    //The operation
    A += value;
    
    Flag((A==0),2,2,2);
}

inline void CPU::adcA(unsigned char value)
{
    int carry = CF();
    
    //The complex flags
    Flag(0,0,((A&0xF)+(value&0xF)+carry>0xF),(A+carry+value>255));
    
    //The operation
    A += value + carry;
    
    Flag((A==0),2,2,2);
}

inline void CPU::subA(unsigned char value)
{
    //The complex flags
    Flag(0,1,((A&0xF)<(value&0xF)),(A<value));
    
    //The operation
    A -= value;
    
    Flag((A==0),2,2,2);
}

inline void CPU::sbcA(unsigned char value)
{
    int carry = CF();
    
    //The complex flags
    Flag(0,1,((A&0xF)<(((int)(value&0xF))+carry)),(A<((int)value+carry)));
    
    //The operation
    A -= (value + carry);
    
    Flag((A==0),2,2,2);
}

//Compare A
inline void CPU::cpA(unsigned char value)
{
    //The complex flags
    Flag((A-value==0),1,((A&0xF)<(value&0xF)),(A<value));
    
    //No operation - Just Flag Setting
}

inline void CPU::Flag(int Z, int N, int H, int C)
{
    //0 - reset
    //1 - set
    //2 - hold
    
    //Create Reset Mask
    unsigned char reset = 0xF0;
    if (Z == 0)
    {
        reset = reset - 128;
    }
    if (N == 0)
    {
        reset = reset - 64;
    }
    if (H == 0)
    {
        reset = reset - 32;
    }
    if (C == 0)
    {
        reset = reset - 16;
    }
    
    //Apply Reset Mask
    F = F & reset;
    
    //Create Set Mask
    reset = 0;
    
    if (Z == 1)
    {
        reset = reset + 128;
    }
    if (N == 1)
    {
        reset = reset + 64;
    }
    if (H == 1)
    {
        reset = reset + 32;
    }
    if (C == 1)
    {
        reset = reset + 16;
    }
    
    //Apply Set Mask
    F = F | reset;
    
    //Done
}


void CPU::cpucycle()
{
    unsigned char currentInstruction;
    int temp, tempb, tempc, tempd;
    unsigned int daa;
    int interruptOn = 0;
    
    //if (halt) { tick+=16666; }
    
    //std::cout << "-:" << std::dec << (long)tick << " " << nextInterrupt << std::endl;
    
    int interrupt = cpuInterrupts();
    long nextCheck = tick + 100;
    
    while (interrupt == 0)
    {
        //if (stop) return;
        
        if (tick > nextCheck)
        {
            nextCheck += 100;
            //std::cout << std::dec << "Check at: " << (long)tick << std::endl;
            interrupt = cpuInterrupts();
        }
    
    currentInstruction = getValue(PC);
        
    unsigned char nextInstruction;
    nextInstruction = ourMemory.getValue2(PC+1);
        
    tick++;
    
    if (!halt) //Halt Stops CPU Cycles
    {
        
    //cpuTest.debugPre(PC);
    
    //printf("PC %4X IN %2X %2X %2X AF=%2X%2X BC=%2X%2X DE=%2X%2X HL=%2X%2X  Flags %i %i %i %i  Top Stack %4X SP: %2X\n",PC, (int)getValue(PC),getValue(PC+1), getValue(PC+2),(int)A,(int)F,(int)B,(int)C,(int)D,(int)E,(int)H,(int)L,ZF(),SF(),HF(),CF(),(int)((int)getValue(SP)|((int)getValue(SP+1))<<8),SP);

    PC++;
    
    switch(currentInstruction)
		    {
		        //LD nn,n
		        case 0x06: B = d8(); tick++; break;
		        case 0x0E: C = d8(); tick++; break;
		        case 0x16: D = d8(); tick++; break;
		        case 0x1E: E = d8(); tick++; break;
		        case 0x26: H = d8(); tick++; break;
		        case 0x2E: L = d8(); tick++; break;
        
		        //LD r1,r2
		        case 0x7F:
		            //A = A;
		        break;
            
		        case 0x78:
		            A = B;
		            break;
            
		        case 0x79:
		            A = C;
		            break;
            
		        case 0x7A:
		            A = D;
		            break;
            
		        case 0x7B:
		            A = E;
		            break;
            
		        case 0x7C:
		            A = H;
		            break;
            
		        case 0x7D:
		            A = L;
		            break;
            
		        case 0x7E:
		            A = getValue(HL());  tick++;        
		            break;
            
		        case 0x40:
		            //B = B;
		            break;
            
		        case 0x41:
		            B = C;
		            break;
            
		        case 0x42:
		            B = D;
		            break;
            
		        case 0x43:
		            B = E;
		            break;
            
		        case 0x44:
		            B = H;
		            break;
            
		        case 0x45:
		            B = L;
		            break;
            
		        case 0x46:
		            B = getValue(HL()); tick++;
		            break;
            
		        case 0x48:
		            C = B;
		            break;
            
		        case 0x49:
		            //C = C;
		            break;
            
		        case 0x4A:
		            C = D;
		            break;
            
		        case 0x4B:
		            C = E;
		            break;
            
		        case 0x4C:
		            C = H;
		            break;
            
		        case 0x4D:
		            C = L;
		            break;
            
		        case 0x4E:
		            C = getValue(HL()); tick++;
		            break;
            
		        case 0x50:
		            D = B;
		            break;
            
		        case 0x51:
		            D = C;
		            break;
            
		        case 0x52:
		            //D = D;
		            break;
            
		        case 0x53:
		            D = E;
		            break;
            
		        case 0x54:
		            D = H;
		            break;
            
		        case 0x55:
		            D = L;
		            break;
            
		        case 0x56:
		            D = getValue(HL()); tick++;
		            break;
            
		        case 0x58:
		            E = B;
		            break;
            
		        case 0x59:
		            E = C;
		            break;
            
		        case 0x5A:
		            E = D;
		            break;
            
		        case 0x5B:
		            //E = E
		            break;
            
		        case 0x5C:
		            E = H;
		            break;
            
		        case 0x5D:
		            E = L;
		            break;
            
		        case 0x5E:
		            E = getValue(HL()); tick++;
		            break;
            
		        case 0x60:
		            H = B;
		            break;
            
		        case 0x61:
		            H = C;
		            break;
            
		        case 0x62:
		            H = D;
		            break;
            
		        case 0x63:
		            H = E;
		            break;
            
		        case 0x64:
		            //H = H;
		            break;
            
		        case 0x65:
		            H = L;
		            break;
            
		        case 0x66:
		            H = getValue(HL()); tick++;
		            break;
            
		        case 0x68:
		            L = B;
		            break;
            
		        case 0x69:
		            L = C;
		            break;
            
		        case 0x6A:
		            L = D;
		            break;
            
		        case 0x6B:
		            L = E;
		            break;
            
		        case 0x6C:
		            L = H;
		            break;
            
		        case 0x6D:
		            //L = L;
		            break;
            
		        case 0x6E:
		            L = getValue(HL()); tick++;
		            break;
            
		        case 0x70:
		            setValue(HL(), B); tick++;
		            break;
            
		        case 0x71:
		            setValue(HL(), C); tick++;
		            break;
            
		        case 0x72:
		            setValue(HL(), D); tick++;
		            break;
            
		        case 0x73:
		            setValue(HL(), E); tick++;
		            break;
            
		        case 0x74:
		            setValue(HL(), H); tick++;
		            break;
            
		        case 0x75:
		            setValue(HL(), L); tick++;
		            break;
            
		        case 0x36:
		            setValue(HL(), d8()); tick+=2;
		            break;
            
		        //LD A,n
		        //case 0x7F: A = A; break;
		        //case 0x78: A = B; break;
		        //case 0x79: A = C; break;
		        //case 0x7A: A = D; break;
		        //case 0x7B: A = E; break;
		        //case 0x7C: A = H; break;
		        //case 0x7D: A = L; break;
		        case 0x0A: A = getValue(BC()); tick++; break;
		        case 0x1A: A = getValue(DE()); tick++; break;
		        //case 0x7E: A = getValue(HL()); break;
		        case 0xFA: A = a16();  tick+=3; break;
		        case 0x3E: A = d8();  tick++; break;
            
		        //LD n,A
		        //case 0x7F: A = A; break;
		        case 0x47: B = A; break;
		        case 0x4F: C = A; break;
		        case 0x57: D = A; break;
		        case 0x5F: E = A; break;
		        case 0x67: H = A; break;
		        case 0x6F: L = A; break;
		        case 0x02: setValue(BC(),A); tick++; break;
		        case 0x12: setValue(DE(),A); tick++; break;
		        case 0x77: setValue(HL(),A); tick++; break;
		        case 0xEA: setA16(A); tick+=3; break;
        
		        //LD A,(C)
		        case 0xF2: A = getValue(0xFF00+C); tick++; break;
        
		        //LD (C),A
		        case 0xE2: setValue(0xFF00+C, A); tick++; break;
        
		        //LDD A,(HL-)
		        case 0x3A: A = getValue(HL()); setHL(HL() - 1); tick++; break;
        
		        //LDD (HL-),A
		        case 0x32: setValue(HL(), A); setHL(HL() - 1); tick++; break;
        
		        //LD A,(HL+)
		        case 0x2A: A = getValue(HL()); setHL(HL() + 1); tick++; break;
        
		        //LDI (HL), A
		        case 0x22: setValue(HL(), A); setHL(HL() + 1); tick++; break;
        
		        //LDH (n), A
		        case 0xE0: setA8(A); tick+=2; break;
        
		        //LDH A,(n)
				case 0xF0: A = getValue(0xFF00 + getValue(PC)); PC++;
				tick += 2; break;
            
		        //LD n,nn
		        case 0x01: setBC(d16()); tick+=2; break;
		        case 0x11: setDE(d16()); tick+=2; break;
		        case 0x21: setHL(d16()); tick+=2; break;
		        case 0x31: SP = d16(); tick+=2; break;
        
		        //LD SP,HL
		        case 0xF9: SP = HL(); tick++; break;
        
		        //LDHL SP,n - Wierd One... C & H flags on 16 bit maths
		        case 0xF8: temp = r8(); setHL(SP+temp); Flag(0,0,((SP&0xF)+(temp&0xf) > 0xF),((int)(SP&0xFF)+(temp&0xff) > 0xFF));  tick+=2; break;
            
		        //LD (nn), SP
		        case 0x08: temp = d16(); setValue((int)temp,SP); setValue((int)temp+1,SP>>8); tick+=4; break;
				
		        //PUSH nn
		        case 0xF5: push(AF(),'D'); tick+=3; break;
		        case 0xC5: push(BC(),'D'); tick+=3; break;
		        case 0xD5: push(DE(),'D'); tick+=3; break;
		        case 0xE5: push(HL(),'D'); tick+=3; break;
        
		        //POP nn
		        case 0xF1: setAF(pop('D')); tick+=2; break;
		        case 0xC1: setBC(pop('D')); tick+=2; break;
		        case 0xD1: setDE(pop('D')); tick+=2; break;
		        case 0xE1: setHL(pop('D')); tick+=2; break;
            
		        //8 Bit ALU
		        //ADD A, n
		        case 0x87: addA(A); break;
		        case 0x80: addA(B); break;
		        case 0x81: addA(C); break;
		        case 0x82: addA(D); break;
		        case 0x83: addA(E); break;
		        case 0x84: addA(H); break;
		        case 0x85: addA(L); break;
		        case 0x86: addA(getValue(HL())); tick++; break;
		        case 0xC6: addA(d8()); tick++; break;
        
		        //ADC A, n
		        case 0x8F: adcA(A); break;
		        case 0x88: adcA(B); break;
		        case 0x89: adcA(C); break;
		        case 0x8A: adcA(D); break;
		        case 0x8B: adcA(E); break;
		        case 0x8C: adcA(H); break;
		        case 0x8D: adcA(L); break;
		        case 0x8E: adcA(getValue(HL())); tick++; break;
		        case 0xCE: adcA(d8()); tick++; break;
        
		        //SUB A, n
		        case 0x97: subA(A); break;
		        case 0x90: subA(B); break;
		        case 0x91: subA(C); break;
		        case 0x92: subA(D); break;
		        case 0x93: subA(E); break;
		        case 0x94: subA(H); break;
		        case 0x95: subA(L); break;
		        case 0x96: subA(getValue(HL())); tick++; break;
		        case 0xD6: subA(d8()); tick++; break;
            
		        //SBC A, n
		        case 0x9F: sbcA(A); break;
		        case 0x98: sbcA(B); break;
		        case 0x99: sbcA(C); break;
		        case 0x9A: sbcA(D); break;
		        case 0x9B: sbcA(E); break;
		        case 0x9C: sbcA(H); break;
		        case 0x9D: sbcA(L); break;
		        case 0x9E: sbcA(getValue(HL())); tick++; break;
				case 0xDE: sbcA(d8()); tick++; break;
        
		        //AND n
		        case 0xA7: A = A & A; Flag(A==0,0,1,0); break;
		        case 0xA0: A = A & B; Flag(A==0,0,1,0); break;
		        case 0xA1: A = A & C; Flag(A==0,0,1,0); break;
		        case 0xA2: A = A & D; Flag(A==0,0,1,0); break;
		        case 0xA3: A = A & E; Flag(A==0,0,1,0); break;
		        case 0xA4: A = A & H; Flag(A==0,0,1,0); break;
		        case 0xA5: A = A & L; Flag(A==0,0,1,0); break;
		        case 0xA6: A = A & getValue(HL()); Flag(A==0,0,1,0); tick++; break;
		        case 0xE6: A = A & d8(); Flag(A==0,0,1,0); tick++; break;
            
		        //OR n
		        case 0xB7: A = A | A; Flag(A==0,0,0,0); break;
		        case 0xB0: A = A | B; Flag(A==0,0,0,0); break;
		        case 0xB1: A = A | C; Flag(A==0,0,0,0); break;
		        case 0xB2: A = A | D; Flag(A==0,0,0,0); break;
		        case 0xB3: A = A | E; Flag(A==0,0,0,0); break;
		        case 0xB4: A = A | H; Flag(A==0,0,0,0); break;
		        case 0xB5: A = A | L; Flag(A==0,0,0,0); break;
		        case 0xB6: A = A | getValue(HL()); Flag(A==0,0,0,0); tick++; break;
		        case 0xF6: A = A | d8(); Flag(A==0,0,0,0); tick++; break;
        
		        //XOR n
		        case 0xAF: A = A ^ A; Flag(A==0,0,0,0); break;
		        case 0xA8: A = A ^ B; Flag(A==0,0,0,0); break;
		        case 0xA9: A = A ^ C; Flag(A==0,0,0,0); break;
		        case 0xAA: A = A ^ D; Flag(A==0,0,0,0); break;
		        case 0xAB: A = A ^ E; Flag(A==0,0,0,0); break;
		        case 0xAC: A = A ^ H; Flag(A==0,0,0,0); break;
		        case 0xAD: A = A ^ L; Flag(A==0,0,0,0); break;
		        case 0xAE: A = A ^ getValue(HL()); Flag(A==0,0,0,0); tick++; break;
		        case 0xEE: A = A ^ d8(); Flag(A==0,0,0,0); tick++; break;
        
		        //CP n
		        case 0xBF: cpA(A); break;
		        case 0xB8: cpA(B); break;
		        case 0xB9: cpA(C); break;
		        case 0xBA: cpA(D); break;
		        case 0xBB: cpA(E); break;
		        case 0xBC: cpA(H); break;
		        case 0xBD: cpA(L); break;
		        case 0xBE: cpA(getValue(HL())); tick++; break;
		        case 0xFE: cpA(d8()); tick++; break;
        
		        //INC n
		        case 0x3C: A++; Flag(A==0,0,(A&0xF)==0,2); break;
		        case 0x04: B++; Flag(B==0,0,(B&0xF)==0,2); break;
		        case 0x0C: C++; Flag(C==0,0,(C&0xF)==0,2); break;
		        case 0x14: D++; Flag(D==0,0,(D&0xF)==0,2); break;
		        case 0x1C: E++; Flag(E==0,0,(E&0xF)==0,2); break;
		        case 0x24: H++; Flag(H==0,0,(H&0xF)==0,2); break;
		        case 0x2C: L++; Flag(L==0,0,(L&0xF)==0,2); break;
		        case 0x34: setValue(HL(), getValue(HL())+1); Flag(getValue(HL())==0,0,(getValue(HL())&0xF)==0,2); tick+=2; break;
        
		        //DEC n
		        case 0x3D: A--; Flag(A==0,1,(A&0xF)==0xF,2); break;
		        case 0x05: B--; Flag(B==0,1,(B&0xF)==0xF,2); break;
		        case 0x0D: C--; Flag(C==0,1,(C&0xF)==0xF,2); break;
		        case 0x15: D--; Flag(D==0,1,(D&0xF)==0xF,2); break;
		        case 0x1D: E--; Flag(E==0,1,(E&0xF)==0xF,2); break;
		        case 0x25: H--; Flag(H==0,1,(H&0xF)==0xF,2); break;
		        case 0x2D: L--; Flag(L==0,1,(L&0xF)==0xF,2); break;
		        case 0x35: setValue(HL(), getValue(HL())-1); Flag(getValue(HL())==0,1,(getValue(HL())&0xF)==0xF,2); tick+=2; break;
        
		        //16 Bit Arithmetic
		        //ADD HL,n
		        case 0x09: Flag(2,0,((HL()&0xFFF)+(BC()&0xFFF)>0xFFF),(HL()+BC()>0xFFFF)); setHL(HL()+BC()); tick++; break;
		        case 0x19: Flag(2,0,((HL()&0xFFF)+(DE()&0xFFF)>0xFFF),(HL()+DE()>0xFFFF)); setHL(HL()+DE()); tick++; break;
		        case 0x29: Flag(2,0,((HL()&0xFFF)+(HL()&0xFFF)>0xFFF),(HL()+HL()>0xFFFF)); setHL(HL()+HL()); tick++; break;
		        case 0x39: Flag(2,0,((HL()&0xFFF)+(SP&0xFFF)>0xFFF),(HL()+SP>0xFFFF)); setHL(HL()+SP); tick++; break;
            
		        //ADD SP,n - Wierd
		        case 0xE8: temp = r8(); tempb = SP; SP += temp; Flag(0,0,((tempb&0xF)+(temp&0xf) > 0xF),((int)(tempb&0xFF)+(temp&0xff) > 0xFF)); tick+=3; break;
        		//Old - Flag(0,0,(temp&0xFFF)+(SP&0xFFF)>0xFFF,temp+SP>0xFFFF);
		
		        //INC nn
		        case 0x03: setBC(BC()+1); tick++; break;
		        case 0x13: setDE(DE()+1); tick++; break; 
		        case 0x23: setHL(HL()+1); tick++; break; 
		        case 0x33: SP++; tick++; break;
        
		        //DEC nn
		        case 0x0B: setBC(BC()-1); tick++; break;
		        case 0x1B: setDE(DE()-1); tick++; break; 
		        case 0x2B: setHL(HL()-1); tick++; break; 
		        case 0x3B: SP--; tick++; break;
            
		        //Extension - Bit Operators
		        case 0xCB:
		            tick++; //Read Time
		            switch(getValue(PC))
		            {
		                //SWAP
		                case 0x37: A = (A << 4) | (A >> 4); Flag(A==0,0,0,0); break;
		                case 0x30: B = (B << 4) | (B >> 4); Flag(B==0,0,0,0); break;
		                case 0x31: C = (C << 4) | (C >> 4); Flag(C==0,0,0,0); break;
		                case 0x32: D = (D << 4) | (D >> 4); Flag(D==0,0,0,0); break;
		                case 0x33: E = (E << 4) | (E >> 4); Flag(E==0,0,0,0); break;
		                case 0x34: H = (H << 4) | (H >> 4); Flag(H==0,0,0,0); break;
		                case 0x35: L = (L << 4) | (L >> 4); Flag(L==0,0,0,0); break;
		                case 0x36: setValue(HL(),(getValue(HL()) << 4) | (getValue(HL()) >> 4)); Flag(getValue(HL())==0,0,0,0); tick+=2; break;
                    
		                //Rotate Left
		                case 0x07: temp = A; A = A << 1; A = A | temp>>7; Flag(A==0,0,0,temp>>7); break;
		                case 0x00: temp = B; B = B << 1; B = B | temp>>7; Flag(B==0,0,0,temp>>7); break;
		                case 0x01: temp = C; C = C << 1; C = C | temp>>7; Flag(C==0,0,0,temp>>7); break;
		                case 0x02: temp = D; D = D << 1; D = D | temp>>7; Flag(D==0,0,0,temp>>7); break;
		                case 0x03: temp = E; E = E << 1; E = E | temp>>7; Flag(E==0,0,0,temp>>7); break;
		                case 0x04: temp = H; H = H << 1; H = H | temp>>7; Flag(H==0,0,0,temp>>7); break;
		                case 0x05: temp = L; L = L << 1; L = L | temp>>7; Flag(L==0,0,0,temp>>7); break;
		                case 0x06: temp = getValue(HL()); setValue(HL(),(getValue(HL()) << 1)|temp>>7); Flag(getValue(HL())==0,0,0,temp>>7); tick+=2; break;
                    
		                case 0x17: temp = A; A = A << 1;A+=CF(); Flag(A==0,0,0,temp>>7); break;
		                case 0x10: temp = B; B = B << 1;B+=CF(); Flag(B==0,0,0,temp>>7); break;
		                case 0x11: temp = C; C = C << 1;C+=CF(); Flag(C==0,0,0,temp>>7); break;
		                case 0x12: temp = D; D = D << 1;D+=CF(); Flag(D==0,0,0,temp>>7); break;
		                case 0x13: temp = E; E = E << 1;E+=CF(); Flag(E==0,0,0,temp>>7); break;
		                case 0x14: temp = H; H = H << 1;H+=CF(); Flag(H==0,0,0,temp>>7); break;
		                case 0x15: temp = L; L = L << 1;L+=CF(); Flag(L==0,0,0,temp>>7); break;
		                case 0x16: temp = getValue(HL()); setValue(HL(),(getValue(HL()) << 1) + CF()); Flag(getValue(HL())==0,0,0,temp>>7); tick+=2; break;
                
		                //Rotate Right
		                case 0x0F: temp = A; A = A >> 1; A += ((temp&0x1)*128); Flag(A==0,0,0,temp&1); break;
		                case 0x08: temp = B; B = B >> 1; B += ((temp&0x1)*128);Flag(B==0,0,0,temp&1); break;
		                case 0x09: temp = C; C = C >> 1; C += ((temp&0x1)*128);Flag(C==0,0,0,temp&1); break;
		                case 0x0A: temp = D; D = D >> 1; D += ((temp&0x1)*128);Flag(D==0,0,0,temp&1); break;
		                case 0x0B: temp = E; E = E >> 1; E += ((temp&0x1)*128);Flag(E==0,0,0,temp&1); break;
		                case 0x0C: temp = H; H = H >> 1; H += ((temp&0x1)*128);Flag(H==0,0,0,temp&1); break;
		                case 0x0D: temp = L; L = L >> 1; L += ((temp&0x1)*128);Flag(L==0,0,0,temp&1); break;
		                case 0x0E: temp = getValue(HL()); setValue(HL(), (temp >> 1) + ((temp&0x1)*128)); Flag(getValue(HL())==0,0,0,temp&1); tick+=2; break;
                
		                case 0x1F: temp = A; A = A >> 1; A+=(CF()*128); Flag(A==0,0,0,temp&1); break;
		                case 0x18: temp = B; B = B >> 1; B+=(CF()*128); Flag(B==0,0,0,temp&1); break;
		                case 0x19: temp = C; C = C >> 1; C+=(CF()*128); Flag(C==0,0,0,temp&1); break;
		                case 0x1A: temp = D; D = D >> 1; D+=(CF()*128); Flag(D==0,0,0,temp&1); break;
		                case 0x1B: temp = E; E = E >> 1; E+=(CF()*128); Flag(E==0,0,0,temp&1); break;
		                case 0x1C: temp = H; H = H >> 1; H+=(CF()*128); Flag(H==0,0,0,temp&1); break;
		                case 0x1D: temp = L; L = L >> 1; L+=(CF()*128); Flag(L==0,0,0,temp&1); break;
		                case 0x1E: temp = getValue(HL()); setValue(HL(), (temp >> 1)+(CF()*128)); Flag(getValue(HL())==0,0,0,temp&1); tick+=2; break;
                
		                //SLA n - LSB set to 0
		                case 0x27: temp = A; A = A << 1; Flag(A==0,0,0,temp>>7); break;
		                case 0x20: temp = B; B = B << 1; Flag(B==0,0,0,temp>>7); break;
		                case 0x21: temp = C; C = C << 1; Flag(C==0,0,0,temp>>7); break;
		                case 0x22: temp = D; D = D << 1; Flag(D==0,0,0,temp>>7); break;
		                case 0x23: temp = E; E = E << 1; Flag(E==0,0,0,temp>>7); break;
		                case 0x24: temp = H; H = H << 1; Flag(H==0,0,0,temp>>7); break;
		                case 0x25: temp = L; L = L << 1; Flag(L==0,0,0,temp>>7); break;
		                case 0x26: temp = getValue(HL()); setValue(HL(),getValue(HL()) << 1); Flag(getValue(HL())==0,0,0,temp>>7); tick+=2; break;
                    
		                //SRA n - MSB preserved
		                case 0x2F: temp = A; A = A >> 1; A = A | (temp&128); Flag(A==0,0,0,temp&1); break;
		                case 0x28: temp = B; B = B >> 1; B = B | (temp&128); Flag(B==0,0,0,temp&1); break;
		                case 0x29: temp = C; C = C >> 1; C = C | (temp&128); Flag(C==0,0,0,temp&1); break;
		                case 0x2A: temp = D; D = D >> 1; D = D | (temp&128); Flag(D==0,0,0,temp&1); break;
		                case 0x2B: temp = E; E = E >> 1; E = E | (temp&128); Flag(E==0,0,0,temp&1); break;
		                case 0x2C: temp = H; H = H >> 1; H = H | (temp&128); Flag(H==0,0,0,temp&1); break;
		                case 0x2D: temp = L; L = L >> 1; L = L | (temp&128); Flag(L==0,0,0,temp&1); break;
		                case 0x2E: temp = getValue(HL()); setValue(HL(),(getValue(HL()) >> 1)| (temp&128)); Flag(getValue(HL())==0,0,0,temp&1); tick+=2; break;
                    
		                //SRL n - MSB set to 0
		                case 0x3F: temp = A; A = A >> 1; Flag(A==0,0,0,temp&1); break;
		                case 0x38: temp = B; B = B >> 1; Flag(B==0,0,0,temp&1); break;
		                case 0x39: temp = C; C = C >> 1; Flag(C==0,0,0,temp&1); break;
		                case 0x3A: temp = D; D = D >> 1; Flag(D==0,0,0,temp&1); break;
		                case 0x3B: temp = E; E = E >> 1; Flag(E==0,0,0,temp&1); break;
		                case 0x3C: temp = H; H = H >> 1; Flag(H==0,0,0,temp&1); break;
		                case 0x3D: temp = L; L = L >> 1; Flag(L==0,0,0,temp&1); break;
		                case 0x3E: temp = getValue(HL()); setValue(HL(),getValue(HL()) >> 1); Flag(getValue(HL())==0,0,0,temp&1); tick+=2; break;
                    
		                default:
		                    //BIT b, r
		                    temp = getValue(PC);
		                    if (temp >= 0x40 && temp < 0x80)
		                    {
								tempb = (temp-0x40)/0x8; //Affected Bit
		                        tempc = 1 << tempb; //Create bit mask
								
		                        tempd = 0;
		                        switch (temp % 0x8)
		                        {
		                            case 0x0: if ((B&tempc)==0) {tempd = 1;} break;
		                            case 0x1: if ((C&tempc)==0) {tempd = 1;} break;
		                            case 0x2: if ((D&tempc)==0) {tempd = 1;} break;
		                            case 0x3: if ((E&tempc)==0) {tempd = 1;} break;
		                            case 0x4: if ((H&tempc)==0) {tempd = 1;} break;
		                            case 0x5: if ((L&tempc)==0) {tempd = 1;} break;
		                            case 0x6: if ((getValue(HL())&tempc)==0) {tempd = 1;}  tick++; break;
		                            case 0x7: if ((A&tempc)==0) {tempd = 1;} break;
		                        }
                        
		                        Flag(tempd,0,1,2);
		                    }
		                    //RES / SET b, r
		                    if (temp >= 0x80)
		                    {
								tempb = (temp-0x80)/0x8;
								if (tempb < 0x8)
								{
									tempc = 0; //RESET
								}
								else
								{
									tempc = 1; //SET
								}
								tempb = tempb % 0x8; //Filter To Just Bit
                        
		                        switch (temp % 0x8)
		                        {
		                            case 0x0: if (tempc) {B |= (1 << tempb);} else { B &= ~(1<<tempb);} break;
		                            case 0x1: if (tempc) {C |= (1 << tempb);} else { C &= ~(1<<tempb);} break;
		                            case 0x2: if (tempc) {D |= (1 << tempb);} else { D &= ~(1<<tempb);} break;
		                            case 0x3: if (tempc) {E |= (1 << tempb);} else { E &= ~(1<<tempb);} break;
		                            case 0x4: if (tempc) {H |= (1 << tempb);} else { H &= ~(1<<tempb);} break;
		                            case 0x5: if (tempc) {L |= (1 << tempb);} else { L &= ~(1<<tempb);} break;
		                            case 0x6: if (tempc) {setValue(HL(),getValue(HL())|(1 << tempb));} else {setValue(HL(),getValue(HL())&~(1 << tempb));}  tick+=2; break;
		                            case 0x7: if (tempc) {A |= (1 << tempb);} else { A &= ~(1<<tempb);} break;
		                        }
		                    }
                    
		                break;
		            }
		            PC++; //Read CB
		            break;
            
		        //DAA - Decimal Adjust Register A
		        //ADD - SUB last operation is different!
		        //Had to use existing emu code as a ref
		        //GB has it's own custom version of this opcode...
		        case 0x27:
		            daa = A;
		            if (SF() == 0)
		            {
		                //Add
		                if (HF() || ((daa&0x0F) > 9)) { daa+=6;}
		                if (CF() || ((daa>0x9f))) { daa+=0x60; }
		            }
		            else
		            {
		                //Sub
		                if (HF()) {daa = (daa-6)&0xff;}
		                if (CF()) {daa -= 0x60;}
		            }
            
		            temp = CF();
		            if (daa&0x100) {temp = 1;}
            
		            A = daa & 0xff;
            
		            Flag((A==0),2,0,temp);
            
		        break;
            
		        //CPL
		        case 0x2F: A = (unsigned char)~A; Flag(2,1,1,2); break;
        
		        //CCF
		        case 0x3F: Flag(2,0,0,!CF()); break;
            
		        //Set Carry Flag
		        case 0x37: Flag(2,0,0,1); break;
            
		        case 0x00: break; //NOP
				case 0x76:   // HALT
                    halt = 1;
                    if (!interrupts)
                    {
                        PC++; //Hardware Glitch
                    }
					break;
		        case 0x10: stop = 1; PC++; break; //STOP
		        case 0xF3: interrupts = 0; break; //DI
		        case 0xFB: interruptOn = 2; break; //EI - Enable Interrupts - Delayed Response
        
		        //Rotates & Shifts - Wierd According to other implementation FlagZ = 0
		        //Rotate A Left
		        case 0x07: temp = A; A = A << 1; A = A | temp>>7; Flag(0,0,0,temp>>7); break;
		        case 0x17: temp = A; A = A << 1; A+=CF(); Flag(0,0,0,temp>>7); break;
		        //Rotate A Right
		        case 0x0F: temp = A; A = A >> 1; A += ((temp&0x1)*128); Flag(0,0,0,temp&1); break;
		        case 0x1F: temp = A; A = A >> 1; A+=(CF()*128); Flag(0,0,0,temp&1); break;
            
        
		        ///JP nn
        case 0xC3: PC = d16();  tick+=3; break;
        //JP cc,nn
        case 0xC2: temp = d16(); if (ZF() == 0) {PC = temp; tick++; } tick+=2; break;
        case 0xCA: temp = d16(); if (ZF() == 1) {PC = temp; tick++; } tick+=2; break;
        case 0xD2: temp = d16(); if (CF() == 0) {PC = temp; tick++; } tick+=2; break;
        case 0xDA: temp = d16(); if (CF() == 1) {PC = temp; tick++; } tick+=2; break;
        
        case 0xE9: PC = HL(); break; //JP HL
        case 0x18: temp = r8(); PC += temp; tick+=2; break; //JR n
        
        //JR cc,nn
        case 0x20: temp = r8(); if (ZF() == 0) {PC += temp; tick++; } tick++; break;
        case 0x28: temp = r8(); if (ZF() == 1) {PC += temp; tick++; } tick++; break;
        case 0x30: temp = r8(); if (CF() == 0) {PC += temp; tick++; } tick++; break;
        case 0x38: temp = r8(); if (CF() == 1) {PC += temp; tick++; } tick++; break;
        
        //CALL nn
        case 0xCD: temp = d16(); push(PC,'S'); PC = temp; tick+=5; break;
        //CALL cc,nn
        case 0xC4: temp = d16(); if (ZF() == 0) {push(PC,'S'); PC = temp; tick += 3; } tick+=2; break;
        case 0xCC: temp = d16(); if (ZF() == 1) {push(PC,'S'); PC = temp; tick += 3; } tick+=2; break;
        case 0xD4: temp = d16(); if (CF() == 0) {push(PC,'S'); PC = temp; tick += 3; } tick+=2; break;
        case 0xDC: temp = d16(); if (CF() == 1) {push(PC,'S'); PC = temp; tick += 3; } tick+=2; break;
		
		        //RST
		        case 0xC7: push(PC,'S'); PC = 0x00; tick+=3; break;
		        case 0xCF: push(PC,'S'); PC = 0x08; tick+=3; break;
		        case 0xD7: push(PC,'S'); PC = 0x10; tick+=3; break;
		        case 0xDF: push(PC,'S'); PC = 0x18; tick+=3; break;
		        case 0xE7: push(PC,'S'); PC = 0x20; tick+=3; break;
		        case 0xEF: push(PC,'S'); PC = 0x28; tick+=3; break;
		        case 0xF7: push(PC,'S'); PC = 0x30; tick+=3; break;
		        case 0xFF: push(PC,'S'); PC = 0x38; tick+=3; break;
        
		        //RET
		        case 0xC9: PC = pop('S'); tick+=3; break;
        
		        //RET cc
		        case 0xC0: if (ZF() == 0) {PC = pop('S'); tick+=3;} tick++; break;
		        case 0xC8: if (ZF() == 1) {PC = pop('S'); tick+=3;} tick++; break;
		        case 0xD0: if (CF() == 0) {PC = pop('S'); tick+=3;} tick++; break;
		        case 0xD8: if (CF() == 1) {PC = pop('S'); tick+=3;} tick++; break;
        
		        //RETI
		        case 0xD9: PC = pop('S'); interrupts = 1; interrupt = cpuInterrupts(); tick+=3; break;
            
		        default:
					printf("Warning Unrecognised Opcode\n");
					tick--;
		            break;
		    }
        
            //printf("Instruction %2X %2X\n",(int)currentInstruction, (int)nextInstruction);
        
    }
        
    if (interruptOn > 0)
    {
        interruptOn--;
        if (interruptOn == 0)
        {
            interrupts = 1;
            interrupt = cpuInterrupts();
        }
    }
    
    //cpuTest.debugPost(A, B, C, D, E, F, H, L, PC, SP);
        
    }
    
}

inline int CPU::cpuInterrupts()
{
    //Step 1. Flag is Updated
    //Update FF0F
    int doVsync = 0;
    
    //V-Blank
    if (tick >= vsyncTime)
    {
        //std::cout << "Current Tick:" << std::dec << (long)tick << " Expected Tick " << (long)vsyncTime << "  LY: " << (int)ourVideo.getLY() << std::endl;
        
        vsyncTime += 17556;
        ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x1);
        doVsync = 1;
        ourVideo.vSync();
    }
    
    //LCDC (STAT)
    unsigned char FF41 = ourMemory.getValue(0xFF41);
    unsigned char FF45 = ourMemory.getValue(0xFF45);
    int currentMode = FF41&3;
    int LY = ourVideo.getLY();
    
    if (
        ((FF41&8)&&currentMode==0) ||
        ((FF41&16)&&currentMode==1) ||
        ((FF41&32)&&currentMode==2) ||
        ((FF41&64)&&(FF41&4)&&LY==FF45)||
        ((FF41&64)&&((FF41&4)==0)&&LY!=FF45)
        )
    {
        ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x2);
        //std::cout << "LCDC STAT" << std::endl;
    }
    
    //Timer Overflow
    
    unsigned char FF07 = ourMemory.getValue(0xFF07);
    int timerOn = (FF07>>2)&1;
    int timerMode = FF07&3;
    
    //Move Forward - Detect if overflows
    long difference = tick - lastTimerUpdate;
    timeAccumulator += difference;
    
    while (timerOn && timerMode == 0 && timeAccumulator > 250)
    {
        timeAccumulator -= 250;
        ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF05)+1);
        
        if (ourMemory.getValue(0xFF05) == 0xFF)
        {
            ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF06));
            ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x4);
        }
    }
    
    while (timerOn && timerMode == 1 && timeAccumulator > 4)
    {
        timeAccumulator -= 4;
        ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF05)+1);
        
        if (ourMemory.getValue(0xFF05) == 0xFF)
        {
            ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF06));
            ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x4);
        }
    }
    
    while (timerOn && timerMode == 2 && timeAccumulator > 15)
    {
        timeAccumulator -= 15;
        ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF05)+1);
        
        if (ourMemory.getValue(0xFF05) == 0xFF)
        {
            ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF06));
            ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x4);
        }
    }
    
    while (timerOn && timerMode == 3 && timeAccumulator > 62)
    {
        timeAccumulator -= 62;
        ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF05)+1);
        
        if (ourMemory.getValue(0xFF05) == 0xFF)
        {
            ourMemory.setValue(0xFF05,ourMemory.getValue(0xFF06));
            ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x4);
        }
    }
    
    lastTimerUpdate = tick;
    
    //Serial IO 0x8
    //Not implemented
    
    //Joypad
    if (keyInterrupt)
    {
        keyInterrupt = 0;
        ourMemory.setValue(0xFF0F,ourMemory.getValue(0xFF0F)|0x10);
    }
    
    
    //Step 2. If interrupts are enabled and the flag is set
    unsigned char FF0F = ourMemory.getValue(0xFF0F);
    unsigned char FFFF = ourMemory.getValue(0xFFFF);
    
    if (FF0F&0x1) //Vsync
    {
        halt = 0;
        
        if (interrupts && FFFF&0x1)
        {
            ourMemory.setValue(0xFF0F,(FF0F&0xfe));
            push(PC,'i');
            PC = 0x40;
            //std::cout << "Vsync" << std::endl;
        }
    }
    
    if (FF0F&0x2) //LCDC
    {
        halt = 0;
        
        if (interrupts && FFFF&0x2)
        {
            interrupts = 0;
            ourMemory.setValue(0xFF0F,(FF0F&0xfd));
            push(PC,'i');
            PC = 0x48;
            //std::cout << "LCDC" << std::endl;
        }
    }
    
    if (FF0F&0x4) //Timer
    {
        halt = 0;
        if (interrupts && FFFF&0x4)
        {
            interrupts = 0;
            ourMemory.setValue(0xFF0F,(FF0F&0xfb));
            push(PC,'i');
            PC = 0x50;
            //std::cout << "Timer" << std::endl;
        }
    }
    
    //Serial I/O 0x8
    
    if (FF0F&0x10) //Keyboard
    {
        halt = 0;
        if (interrupts && FFFF&0x10)
        {
            interrupts = 0;
            ourMemory.setValue(0xFF0F,(FF0F&0xef));
            push(PC,'i');
            PC = 0x60;
            //std::cout << "Keyboard" << std::endl;
        }
    }
    
    
    //Misc - HSync
    if (hsyncTime < tick)
    {
        hsyncTime += 114;
        ourVideo.hSync();
    }
    
    return doVsync;
}


#endif