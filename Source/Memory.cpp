//
//  Memory.cpp
//  GB Emulator
//
//  Created by Frank Burton on 01/10/2013.
//
//

#include "main.h"
#include "Memory.h"

extern CPU ourCPU;
extern Video ourVideo;
extern Sound ourSound;
extern long tick;

void Memory::setValue(unsigned int address, unsigned char value)
{
    //std::cout << "Writing to " << std::hex << address << " " << std::hex << (int)value << std::endl;
    
    if (address >= 0xFF80 && address <= 0xFFFE) //High Ram
    {
        Mem[address] = value;
        return;
    }
    
    //RAM Enable
    if (address <= 0x1fff)
    {
        //std::cout << "RAM Enable" << std::endl;
        RAMEnable = 1;
        
        //There are various cart types
        //with various enable switches
        //so lets turn it on no matter what
        //hopefully, no cart will rely on the disable function
    }
    
    //Memory Bank Switch
    if (address >= 0x2000 && address <= 0x3FFF)
    {
        if (mbc1)
        {
            value = value & 31;
            
            if (value == 0)
            {
                value = 1;
            }
            
            //Reset Lower ROM Bank Number
            ROMBank = ROMBank & 224;
            
            //Set New Number
            ROMBank = ROMBank | value;
        }
        if (mbc2)
        {
            value = value & 15;
            ROMBank = value;
        }
        
        //MBC 3 and other unknown carts
        if (mbc1 == 0 && mbc2 == 0)
        {
            value = value & 127;
            if (value == 0)
            {
                value = 1;
            }
            ROMBank = value;
        }
        
        if (ROMBank > maxBank)
        {
            std::cout << "Hardware Failure, Attempted to Load Bank " << (int)ROMBank << " of " << (int)maxBank << std::endl;
            //exit(1);
        }
        
        //std::cout << "ROM Bank Switched to " << (int)ROMBank << std::endl;
        return;
    }
    
    if (address >= 0x4000 && address <= 0x5FFF)
    {
        value = value & 0x3;
        
        if (MemorySelect == 0)
        {
            //std::cout << "ROM Upper Bit Set" << std::endl;
            ROMBank = ROMBank & 31;
            ROMBank = ROMBank | (value << 4);
            if (ROMBank > maxBank)
            {
                //std::cout << "Hardware Failure, Attempted to Load Bank " << (int)ROMBank << " of " << (int)maxBank << std::endl;
                //exit(1);
            }
            
            //std::cout << "ROM Bank Switched to " << (int)ROMBank << std::endl;
        }
        
        if (MemorySelect == 1)
        {
            //std::cout << "RAM Bank Switch to " << std::hex << (int)value << std::endl;
            RAMBank = value;
        }
        return;
    }
    
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        value = value & 0x1;
        //std::cout << "Memory Mode Change to " << (int)value << std::endl;
        MemorySelect = value;
        return;
    }
    
    //RAM Mirror
    if (address >= 0xE000 && address < 0xFE00)
    {
        address -= 0x2000;
    }
    
    //Write To RAM Banks
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (RAMEnable)
        {
            if (mbc2)
            {
                value = value & 0x0f; //Only 4 bit memory
            }
            RAM[address-0xA000+(RAMBank*0x2000)] = value;
            ramWrite = 1;
        }
        else
        {
            //std::cout << "Block Write To RAM" << std::endl;
        }
    }
    
    //Written To VRAM
    if (address >= 0x8000 && address <= 0x97FF)
    {
        Mem[address] = value;
        ourVideo.informVideo(address);
        return;
    }
    
    //Written To Display Maps
    if (address >= 0x9800 && address <=0x9fff)
    {
        Mem[address] = value;
        ourVideo.informVideo(address);
        return;
    }
    
    //Basic Warnings
    if (address < 0x8000)
    {
        //std::cout << "Attempted To Write to " << std::hex << address << " Denied Terminating" << std::endl;
        return;
        //exit(1);
    }
    
    switch (address)
    {
            //Joypad Info - Set mode
        case 0xFF00:
            ourCPU.keyboardselect = (~value) & (32+16);
            Mem[0xFF00] = value;
            
            break;
            
            //FF01 (SB) - Serial Transfer Data
        case 0xFF01:
            //std::cout << "Serial Transfer: " << value << std::endl;
            break;
            //FF02 (SIC) - Start Transfer - Link Cable
            //FF04 (Divider Register)
        case 0xFF04:
            Mem[0xFF04] = 0; //Ignore write value
            ourCPU.divider = tick; //Reset The Output Value
            break;
            
            //FF05 (TIMA)
        case 0xFF05:
            Mem[0xFF05] = value;
            break;
            
            //Timer Control
        case 0xFF07:
            Mem[0xFF07] = value;
            break;
            
            //Interupt Flag - 0xFF0F - Type Of Interupt
            //Interupt Flag2 - 0xFFFF- Interupt Enabled
            
            
            //Sound Card - Inform The Sound Card
            
        case 0xFF10:
        case 0xFF11:
        case 0xFF12:
        case 0xFF13:
        case 0xFF14:
        case 0xFF16:
        case 0xFF17:
        case 0xFF18:
        case 0xFF19:
        case 0xFF1A:
        case 0xFF1B:
        case 0xFF1C:
        case 0xFF1D:
        case 0xFF1E:
        case 0xFF20:
        case 0xFF21:
        case 0xFF22:
        case 0xFF23:
        case 0xFF24:
        case 0xFF25:
        case 0xFF26:
            
            Mem[address] = value;
            ourSound.informSound(address, value);
            break;
            
            //Video Comm - Interrupts
        case 0xFF41:
            Mem[address] = value;
            ourVideo.informVideo(address);
            break;
            
            //Video Comm
        case 0xFF40:
        case 0xFF42:
        case 0xFF43:
        case 0xFF44:
        case 0xFF45:
        case 0xFF47:
        case 0xFF48:
        case 0xFF49:
        case 0xFF4A:
        case 0xFF4B:
            
            Mem[address] = value;
            ourVideo.informVideo(address);
            break;
            
            //DMA - Loads the sprite data
        case 0xFF46:
            Mem[address] = value;
            
            if (value > 0xFE)
            {
                std::cout << "DMA ERROR - Invalid Address" << std::endl;
                exit(1);
                break;
            }
            
            dmaTransfer = value;
            dmaTransferProgress = 0;
            dmaLastTick = tick;
            
            break;
            
        case 0xFFFF:
            Mem[address] = value;
            break;
            
        default:
            Mem[address] = value;
            
            //Inform Video if Sprite Data Changes
            if (address >= 0xFE00 && address <= 0xFE9F)
            {
                ourVideo.informVideo(address);
            }
            
            break;
    }
}

unsigned char Memory::getValue(unsigned short address)
{
    unsigned char value = getValue2(address);
    //printf("%x %x\n",(int)address, (int)value);
    
    //DMA Transfer
    while (dmaTransfer != -1 && dmaLastTick < tick)
    {
        setValue(0xfe00+dmaTransferProgress,getValue2(dmaTransfer*0x100+dmaTransferProgress));
        dmaTransferProgress++;
        if (dmaTransferProgress == 0xa1)
        {
            dmaTransfer = -1;
            dmaTransferProgress = 0;
        }
        dmaLastTick++; //Keep In Sync With Ticks Should Take 640 cycles which is approx 671 cycles (hw)
    }
    
    return value;
}

//Intercept calls to the registers
unsigned char Memory::getValue2(unsigned short address)
{
    unsigned char temp;
    
    //ROM Bank 0
    if (address < 0x4000)
    {
        return Mem[address];
    }
    
    //Switchable ROM Bank
    //MBC1 - Switch
    if(address >= 0x4000 && address <= 0x7FFF)
    {
        return ROMMemory[(ROMBank*0x4000)+(address-0x4000)];
    }
    
    //RAM Mirror
    if (address >= 0xE000 && address < 0xFE00)
    {
        address -= 0x2000;
    }
    
    //RAM Banks
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (RAMEnable)
        {
            return RAM[address-0xA000+(RAMBank*0x2000)];
        }
        else
        {
            std::cout << "Block Read From RAM" << std::endl;
        }
    }
    
    //OAM
    if (address >= 0xfe00 && address <= 0xfe9f)
    {
        return Mem[address];
    }
    
    //High RAM
    if (address >= 0xFF80 && address <= 0xFFFE)
    {
        return Mem[address];
    }
    
    //Sound Card
    if (address >= 0xFF10 && address <=0xFF26)
    {
        return ourSound.requestValue(address, Mem[address]); //Ask our Sound Card to modify the last stored value in memory to be correct. Some addresses can't be written to, etc.
    }
    
    //Special Register
    switch (address)
    {
            //FFOO - Joypad Info
        case 0xFF00:
            temp = Mem[0xFF00]&0x30;
            
            if (temp == 0x30)
            {
                temp = temp | (ourCPU.keyDown << 3) | (ourCPU.keyUp << 2) | (ourCPU.keyLeft << 1) | ourCPU.keyRight;
                temp = temp | (ourCPU.keyStart << 3) | (ourCPU.keySelect << 2) | (ourCPU.keyB << 1) | ourCPU.keyA;
            }
            
            if (temp == 0x20)
            {
                temp = temp | (ourCPU.keyDown << 3) | (ourCPU.keyUp << 2) | (ourCPU.keyLeft << 1) | ourCPU.keyRight;
            }
            if (temp == 0x10)
            {
                temp = temp | (ourCPU.keyStart << 3) | (ourCPU.keySelect << 2) | (ourCPU.keyB << 1) | ourCPU.keyA;
            }
            if (temp == 0x0)
            {
                temp = 0x1f;
            }
            
            return temp;
            break;
            
            //FF01 (SB) - Serial Transfer Data
            
            //FF02 (SIC) - Start Transfer - Link Cable
            //FF04 (Divider Register)
        case 0xFF04:
            return ((tick - ourCPU.divider) % 0xFF);
            break;
            
            //FF05 (TIMA)
        case 0xFF05:
            return Mem[0xFF05];
            break;
            
            //Sound Card
            case 0xFF11:
                return Mem[0xFF11]|63; //Can't fully read
            break;
            
        case 0xFF13:
                return 0xFF; //Can't be read
            break;
            
        case 0xFF14:
            return Mem[0xFF14]|191; //Only Bit 6 can be read
            break;
            
        case 0xFF10:
        case 0xFF12:
        case 0xFF16:
        case 0xFF17:
        case 0xFF18:
        case 0xFF19:
        case 0xFF1A:
        case 0xFF1B:
        case 0xFF1C:
        case 0xFF1D:
        case 0xFF1E:
        case 0xFF20:
        case 0xFF21:
        case 0xFF22:
        case 0xFF23:
        case 0xFF24:
        case 0xFF25:
        case 0xFF26:
            //std::cout << "Sound Card Read " << std::hex << (int)address << std::endl;
            return Mem[address];
            break;
            
            //Video Card Status
        case 0xFF41:
            return ourVideo.getFF41();
            break;
            
        case 0xFF44: //Current writing to line...
            return ourVideo.getLY();
            break;
            
        case 0xFF0F:
            return Mem[0xFF0F];
        break;
            
        case 0xFFFF:
            return Mem[0xFFFF];
            break;
            
        default:
            return Mem[address];
            break;
    }
}

void Memory::loadrom(std::string romFileName)
{
    
    for (int i = 0; i < 0xFFFF; i++)
    {
        Mem[i] = 0; //Blank Memory
    }
    
    saveFileName = "";
    
    //Save file name - Remove .gb add .sav
    for (int i = 0; i < romFileName.size(); i++)
    {
        char letter = romFileName.at(i);
        if (letter == '.')
        {
            break;
        }
        saveFileName.push_back(letter);
    }
    saveFileName.push_back('.');
    saveFileName.push_back('s');
    saveFileName.push_back('a');
    saveFileName.push_back('v');
    
    //Rom Loading
    FILE * romFile;
    romFile = fopen (romFileName.c_str(),"r");
    
    if (romFile == NULL)
    {
        std::cout << "No Rom File Selected" << std::endl;
        ready = 0;
        return;
    }

        fseek (romFile, 0 , SEEK_END);
        long fileSize = ftell (romFile);
        rewind (romFile);
        
        maxBank = fileSize / 16383;
    
        if (ROMMemory != NULL)
        {
            free(ROMMemory);
        }
        ROMMemory = (unsigned char*)malloc(sizeof(char) * fileSize);
        for (int i = 0; i < fileSize; i++)
        {
            ROMMemory[i] = 0; //Blank Memory
        }
        
        fread((void*)ROMMemory, 1, fileSize,romFile);
        
        //Load Bank 0 + Bank 1
        for (int i = 0; i < 32768; i++)
        {
            Mem[i] = ROMMemory[i];
        }
        cartType = (int)Mem[0x147];
        
        std::cout << "Loaded Game Title: " << &Mem[0x134] << std::endl;
        std::cout << "ROM Size: " << fileSize << std::endl;
    std::cout << "Memory Type: " << std::hex << cartType << std::endl;
    
    mbc1 = 1; //We are hopeful!
    mbc2 = 0;
    mbc3 = 0;
    
    if (cartType > 0x3 && cartType < 0x7)
    {
        mbc1 = 0;
        mbc2 = 1;
        mbc3 = 0;
    }
    
    if (cartType > 0xd && cartType < 0x15)
    {
        mbc1 = 0;
        mbc2 = 0;
        mbc3 = 1;
    }
    
    RAMBank = 0;
    ROMBank = 1;
    RAMEnable = 1;
    MemorySelect = 0;
    
    //Load Save File
    FILE *ramFile;
    ramFile = fopen(saveFileName.c_str(), "r");
    if (ramFile != NULL)
    {
        fread((void*)RAM, 1, 0x8000,ramFile);
        fclose(ramFile);
    }
    
    ready = 1;
    ourSound.startSound();
}


void Memory::inc(unsigned int address)
{
    setValue(address,getValue(address)+1);
}

void Memory::dec(unsigned int address)
{
    setValue(address,getValue(address)-1);
}

Memory::Memory()
{
    ramWrite = 0;
    ready = 0;
    ROMMemory = NULL;
}

void Memory::saveRAM()
{
    if (ramWrite)
    {
        FILE * saveFile;
        saveFile = fopen (saveFileName.c_str(),"w");
        fwrite(RAM,1,sizeof(RAM),saveFile);
        fclose(saveFile);
        ramWrite = 0;
    }
}

