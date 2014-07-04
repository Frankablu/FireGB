//
//  Memory.h
//  GB Emulator
//
//  Created by Frank Burton on 01/10/2013.
//
//

#ifndef __GB_Emulator__Memory__
#define __GB_Emulator__Memory__

#include <iostream>

//Models The GB Memory

class Memory
{
private:
    unsigned char RAM[0x8000];
    unsigned char *ROMMemory;
    unsigned char ROMBank;
    unsigned char maxBank;
    unsigned char RAMBank;
    
    int cartType;
    int RAMEnable;
    int MemorySelect;
    
    int mbc1;
    int mbc2;
    int mbc3;
    
    int ramWrite;
    std::string saveFileName;
    
public:
    Memory();
    
    unsigned char Mem[0xFFFF + 2];
    void loadrom(std::string romFileName);
    unsigned char getValue(unsigned short address);
    void setValue(unsigned int address, unsigned char value);
    void inc(unsigned int address);
    void dec(unsigned int address);
    
    unsigned char getValue2(unsigned short address);
    
    int dmaTransfer;
    int dmaTransferProgress;
    long dmaLastTick;
    
    void saveRAM();
    
    int ready;
    
};

#endif /* defined(__GB_Emulator__Memory__) */
