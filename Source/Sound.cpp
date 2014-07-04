//
//  Sound.cpp
//  GB Emulator
//
//  Created by Frank on 07/09/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "main.h"

#ifndef SOUND
#define SOUND

extern CPU ourCPU;
extern Memory ourMemory;
extern Sound ourSound;

extern long tick;

long soundTick;

//Sound Channel 1
int Sound1InternalOn;
int Sound1SweepTime;
long Sound1NextSweepTime;
int Sound1SweepDirection;
int Sound1SweepShifts;
int Sound1CurrentSweepShift;
int Sound1WavePatternDuty;
int Sound1SoundLength;
long Sound1SoundLengthStop;
int Sound1EnvelopeVolume;
int Sound1EnvelopeVolumeInitial;
long Sound1NextEnvelopeVolumeShift;
int Sound1EnvelopeDirection;
int Sound1EnvelopeSweepsLeft;
int Sound1Frequency;
double Sound1FrequencyReal;
int Sound1Restart;
int Sound1CounterSelect;
double Sound1SoundPlace;
double Sound1WaveDutyPeriod;

//Sound Channel 2
int Sound2InternalOn;
int Sound2WavePatternDuty;
int Sound2SoundLength;
long Sound2SoundLengthStop;
int Sound2EnvelopeVolume;
int Sound2EnvelopeVolumeInitial;
long Sound2NextEnvelopeVolumeShift;
int Sound2EnvelopeDirection;
int Sound2EnvelopeSweepsLeft;
int Sound2Frequency;
double Sound2FrequencyReal;
int Sound2Restart;
int Sound2CounterSelect;
double Sound2SoundPlace;
double Sound2WaveDutyPeriod;

//Sound Channel 3
int Sound3On;
int Sound3SoundLength;
long Sound3SoundLengthStop;
int Sound3Frequency;
double Sound3FrequencyReal;
int Sound3Restart;
int Sound3CounterSelect;
int Sound3InternalOn;
int Sound3OutputLevel;
double Sound3SoundPlace;

//Sound Channel 4
int Sound4InternalOn;
int Sound4SoundLength;
long Sound4SoundLengthStop;
int Sound4EnvelopeVolume;
int Sound4EnvelopeVolumeInitial;
long Sound4NextEnvelopeVolumeShift;
int Sound4EnvelopeDirection;
int Sound4EnvelopeSweepsLeft;
int Sound4Restart;
int Sound4CounterSelect;
int Sound4ShiftClockFrequency;
int Sound4LFSRSize;
int Sound4LFSRDividingRatio;
int Sound4SoundPlace;

bool Sound4PreGenerated8[256];
bool Sound4PreGenerated16[65536];

double SoundChannel1Volume;
double SoundChannel2Volume;

double Sound1VolumeScale;
double Sound2VolumeScale;
double Sound3VolumeScale;
double Sound4VolumeScale;
int SoundConnections;
int MasterSoundSwitch = 1;

void sound_callback(void *data, Uint8 *stream, int len);
inline double audioWave1(double x);
inline double audioWave2(double x);
inline double audioWave3(double x);
inline double audioWave4();

int audioMute = 0; //Emulator Mute

//Audio Function
void sound_callback(void *data, Uint8 *stream, int len)
{
    ourSound.updateSound();
    
    int16_t *stream16 = (int16_t*)stream;
    double maxSoundPlace1 = 6.283184 / (2.0 * 3.141592 * Sound1FrequencyReal / 44100.0);
    double maxSoundPlace2 = 6.283184 / (2.0 * 3.141592 * Sound2FrequencyReal / 44100.0);
    double maxSoundPlace3 = 6.283184 / (2.0 * 3.141592 * Sound3FrequencyReal / 44100.0);
    
    if (MasterSoundSwitch && audioMute == 0)
    {
    for (int i = 0; i < len/2; i++)
    {
        Sound1SoundPlace++;
        Sound2SoundPlace++;
        Sound3SoundPlace++;
        
        stream16[i] = 100.0 * Sound1VolumeScale * Sound1EnvelopeVolume * Sound1InternalOn * audioWave1(Sound1SoundPlace * 2.0 * 3.141592 * Sound1FrequencyReal / 44100.0);
        
        stream16[i] += 100.0 * Sound2VolumeScale * Sound2EnvelopeVolume * Sound2InternalOn * audioWave1(Sound2SoundPlace * 2.0 * 3.141592 * Sound2FrequencyReal / 44100.0);
        
        stream16[i] += 100.0 * Sound3VolumeScale * Sound3InternalOn * audioWave3(Sound3SoundPlace * 2.0 * 3.141592 * Sound3FrequencyReal / 44100.0);
        
        stream16[i] += 100.0 * Sound4VolumeScale * Sound4EnvelopeVolume * Sound4InternalOn * audioWave4();
        
        if (Sound1SoundPlace > maxSoundPlace1)
        {
            Sound1SoundPlace = 0.0;
        }
        if (Sound2SoundPlace > maxSoundPlace2)
        {
            Sound2SoundPlace = 0.0;
        }
        if (Sound3SoundPlace > maxSoundPlace3)
        {
            Sound3SoundPlace = 0.0;
        }
    }
    }
    else
    {
        //No Sound
        for (int i = 0; i < len/2; i++)
        {
            stream16[i] = 0;
        }
    }
}

inline double audioWave1(double x)
{
    if (x < Sound1WaveDutyPeriod)
    {
        return -1.0;
    }
    
    return 1.0;
}

inline double audioWave2(double x)
{
    if (x < Sound2WaveDutyPeriod)
    {
        return -1.0;
    }
    
    return 1.0;
}

inline double audioWave3(double x)
{
    //30 Places for Samples
    int place = x / 0.209439;
    if (place > 29)
    {
        place = 29;
    }
    //4 Bit Samples
    if (place % 2 == 0)
    {
        //Upper bits
        return (ourMemory.Mem[0xFF30+(place/2)]>>4)>>Sound3OutputLevel; //Speed is important in sound systems so direct access (Avoid buffer underrun)
    }
    else
    {
        //Lower bits
        return (ourMemory.Mem[0xFF30+(place/2)]&15)>>Sound3OutputLevel;
        
    }
}

//Fake White Noise Produced by a LFSR
inline double audioWave4()
{
    int updateCount;
    int output;
    if (Sound4LFSRDividingRatio == 0)
    {
        updateCount = 22;
    }
    else
    {
        updateCount = 11/Sound4LFSRDividingRatio;
    }
    updateCount = updateCount * pow(0.5,Sound4ShiftClockFrequency);
    Sound4SoundPlace += updateCount;
    
    if (Sound4LFSRSize)
    {
        //Size 8
        if (Sound4SoundPlace > 255)
        {
            Sound4SoundPlace = 0;
        }
        output = Sound4PreGenerated8[Sound4SoundPlace];
    }
    else
    {
        //Size 16
        if (Sound4SoundPlace > 65535)
        {
            Sound4SoundPlace = 0;
        }
        output = Sound4PreGenerated16[Sound4SoundPlace];

    }
    
    if (output)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void Sound::startSound()
{
    SDL_AudioSpec *desired, *obtained;
    SDL_AudioSpec *hardware_spec;
    desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
    obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
    desired->freq=44100;
    desired->format=AUDIO_S16SYS;
    desired->channels=1;
    desired->samples=4096;
    desired->callback=sound_callback;
    desired->userdata=NULL;
    if ( SDL_OpenAudio(desired, obtained) < 0 ){
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    }
    free(desired);
    hardware_spec=obtained;
    SDL_PauseAudio(0);
    soundTick = SDL_GetTicks();
    Sound1SoundPlace = 0;
    Sound2SoundPlace = 0;
    Sound3SoundPlace = 0;
    Sound4SoundPlace = 0;
    
    //Create Fake White Noise Tables
    int lastBit, outputBit;
    unsigned int Sound4LFSRState8=255;
    unsigned int Sound4LFSRState16=65535;
    
    for (int i = 0; i < 256; i++)
    {
        lastBit = Sound4LFSRState8&1;
        Sound4LFSRState8 = Sound4LFSRState8 >> 1;
        outputBit = Sound4LFSRState8&1;
        Sound4LFSRState8 += (128 * (lastBit^outputBit));
        Sound4PreGenerated8[i] = outputBit;
    }
    
    for (int i = 0; i < 65536; i++)
    {
        lastBit = Sound4LFSRState16&1;
        Sound4LFSRState16 = Sound4LFSRState16 >> 1;
        outputBit = Sound4LFSRState16&1;
        Sound4LFSRState16 += (128 * (lastBit^outputBit));
        Sound4PreGenerated16[i] = outputBit;
    }
}

void Sound::updateSound()
{
    soundTick = SDL_GetTicks();
    
    //Sound Channel 1
    //Frequency Shift Logic
    while (Sound1NextSweepTime < soundTick && Sound1SweepShifts > 0 && Sound1CurrentSweepShift < Sound1SweepShifts)
    {
        Sound1NextSweepTime += Sound1SweepTime * 8;
        Sound1CurrentSweepShift++;
        if (!Sound1SweepDirection)
        {
            //Addition
            Sound1FrequencyReal += Sound1FrequencyReal / pow(2.0,Sound1CurrentSweepShift);
            if (Sound1FrequencyReal > 131000) //Exceed Max Frequency
            {
                Sound1InternalOn = 0;
            }
        }
        else
        {
            //Subtraction
            Sound1FrequencyReal -= Sound1FrequencyReal / pow(2.0,Sound1CurrentSweepShift);
            if (Sound1FrequencyReal < 64) //Min Frequency
            {
                Sound1FrequencyReal = 64;
            }
        }
    }
    
    //Volume Shift Logic
    while (Sound1EnvelopeSweepsLeft > 0 && Sound1NextEnvelopeVolumeShift < soundTick)
    {
        Sound1EnvelopeSweepsLeft--;
        Sound1NextEnvelopeVolumeShift += 15;
        if (!Sound1EnvelopeDirection)
        {
            //Addition
            Sound1EnvelopeVolume++;
            if (Sound1EnvelopeVolume > 16)
            {
                Sound1EnvelopeVolume = 16;
            }
        }
        else
        {
            //Subtraction
            Sound1EnvelopeVolume--;
            if (Sound1EnvelopeVolume < 0)
            {
                Sound1EnvelopeVolume = 0;
            }
        }
    }
    
    //Sound Channel 1 On - Sound Length / Counter Select
    Sound1InternalOn = 0;
    if (soundTick < Sound1SoundLengthStop || Sound1CounterSelect == 0)
    {
        Sound1InternalOn = 1;
    }
    
    //Sound Channel 2
    //Volume Shift Logic
    while (Sound2EnvelopeSweepsLeft > 0 && Sound2NextEnvelopeVolumeShift < soundTick)
    {
        Sound2EnvelopeSweepsLeft--;
        Sound2NextEnvelopeVolumeShift += 15;
        if (!Sound2EnvelopeDirection)
        {
            //Addition
            Sound2EnvelopeVolume++;
            if (Sound2EnvelopeVolume > 16)
            {
                Sound2EnvelopeVolume = 16;
            }
        }
        else
        {
            //Subtraction
            Sound2EnvelopeVolume--;
            if (Sound2EnvelopeVolume < 0)
            {
                Sound2EnvelopeVolume = 0;
            }
        }
    }
    
    //Sound Channel 4
    //Volume Shift Logic
    while (Sound4EnvelopeSweepsLeft > 0 && Sound4NextEnvelopeVolumeShift < soundTick)
    {
        Sound4EnvelopeSweepsLeft--;
        Sound4NextEnvelopeVolumeShift += 15;
        if (!Sound4EnvelopeDirection)
        {
            //Addition
            Sound4EnvelopeVolume++;
            if (Sound4EnvelopeVolume > 16)
            {
                Sound4EnvelopeVolume = 16;
            }
        }
        else
        {
            //Subtraction
            Sound4EnvelopeVolume--;
            if (Sound4EnvelopeVolume < 0)
            {
                Sound4EnvelopeVolume = 0;
            }
        }
    }
    
    //Sound Channel 2 On - Sound Length / Counter Select
    Sound2InternalOn = 0;
    if (soundTick < Sound2SoundLengthStop || Sound2CounterSelect == 0)
    {
        Sound2InternalOn = 1;
    }
    
    //Sound Channel 3 On - Sound Length / Counter Select
    Sound3InternalOn = 0;
    if (soundTick < Sound2SoundLengthStop || Sound3CounterSelect == 0)
    {
        Sound3InternalOn = 1;
    }
    
    if (Sound3On == 0) //Extra On/Off Switch on this sound channel
    {
        Sound3InternalOn = 0;
    }
    
    
    //Sound Channel 4 On - Sound Length / Counter Select
    Sound4InternalOn = 0;
    if (soundTick < Sound4SoundLengthStop || Sound4CounterSelect == 0)
    {
        Sound4InternalOn = 1;
    }
    
}

void Sound::informSound(unsigned int address, unsigned int value)
{
    soundTick = SDL_GetTicks();
    
    switch (address)
    {
        //Sound Channel 1 - Sweep Register
        case 0xFF10:
            Sound1SweepTime = ((value>>4)&7);
            Sound1NextSweepTime = soundTick + Sound1SweepTime * 8;
            Sound1EnvelopeDirection = (value>>3)&1;
            Sound1EnvelopeSweepsLeft = value&7;
            Sound1CurrentSweepShift = 0; //Might only be on reset
            break;
        
        //Sound Channel 1 - Sound Length - Sound Duty
        case 0xFF11:
            Sound1WavePatternDuty = value>>6;
            //Sound Duty is how long is spent at -1 in the wave compared to 1
            switch (Sound1WavePatternDuty)
            {
                case 0:
                    Sound1WaveDutyPeriod = 0.785398;
                    break;
                    
                case 1:
                    Sound1WaveDutyPeriod = 1.570796;
                    break;
                    
                case 2:
                    Sound1WaveDutyPeriod = 3.141592;
                    break;
                    
                case 3:
                    Sound1WaveDutyPeriod = 4.712388;
                    break;
            }
            Sound1SoundLength = (64-(value&63))*4;
        break;
            
        //Sound Channel 1 - Envelope
        case 0xFF12:
            Sound1EnvelopeVolumeInitial = (value>>4)&7;
            if (Sound1EnvelopeVolumeInitial == 0) //Zero mutes immediately otherwise wait till sound reset
            {
                Sound1EnvelopeVolume = 0;
            }
            Sound1EnvelopeDirection = (value>>3)&1;
            Sound1EnvelopeSweepsLeft = value&7;
            Sound1NextEnvelopeVolumeShift = soundTick + 15;
            break;
            
        //Sound Channel 1 - Frequency Low
        case 0xFF13:
            Sound1Frequency = (Sound1Frequency&1792)|value;
            Sound1FrequencyReal = 131072.0/(double)(2048-Sound1Frequency);
            break;
            
        //Sound Channel 1 - Reset Sound - Counter Select - Frequency High
        case 0xFF14:
            Sound1Restart = value>>7;
            Sound1CounterSelect = (value>>6)&1;
            Sound1Frequency = (Sound1Frequency&255)|(((int)(value&7))<<8);
            Sound1FrequencyReal = 131072.0/(double)(2048-Sound1Frequency);
            
            if (Sound1Restart)
            {
                Sound1SoundLengthStop = soundTick + Sound1SoundLength;
                Sound1EnvelopeVolume = Sound1EnvelopeVolumeInitial;
            }
            break;
            
        //Sound Channel 2 - Sound Length - Sound Duty
        case 0xFF16:
            Sound2WavePatternDuty = value>>6;
            //Sound Duty is how long is spent at -1 in the wave compared to 1
            switch (Sound2WavePatternDuty)
            {
                case 0:
                    Sound2WaveDutyPeriod = 0.785398;
                break;
                
                case 1:
                    Sound2WaveDutyPeriod = 1.570796;
                break;
                
                case 2:
                    Sound2WaveDutyPeriod = 3.141592;
                break;
                
                case 3:
                    Sound2WaveDutyPeriod = 4.712388;
                break;
            }
            Sound2SoundLength = (64-(value&63))*4;
            break;
            
        //Sound Channel 2 - Envelope
        case 0xFF17:
            Sound2EnvelopeVolumeInitial = (value>>4)&7;
            if (Sound2EnvelopeVolumeInitial == 0) //Zero mutes immediately otherwise wait till sound reset
            {
                Sound2EnvelopeVolume = 0;
            }
            Sound2EnvelopeDirection = (value>>3)&1;
            Sound2EnvelopeSweepsLeft = value&7;
            Sound2NextEnvelopeVolumeShift = soundTick + 15;
            break;
            
            //Sound Channel 2 - Frequency Low
        case 0xFF18:
            Sound2Frequency = (Sound2Frequency&1792)|value;
            Sound2FrequencyReal = 131072.0/(double)(2048-Sound2Frequency);
            break;
            
            //Sound Channel 2 - Reset Sound - Counter Select - Frequency High
        case 0xFF19:
            Sound2Restart = value>>7;
            Sound2CounterSelect = (value>>6)&1;
            Sound2Frequency = (Sound2Frequency&255)|(((int)(value&7))<<8);
            Sound2FrequencyReal = 131072.0/(double)(2048-Sound2Frequency);
            
            if (Sound2Restart)
            {
                Sound2SoundLengthStop = soundTick + Sound2SoundLength;
                Sound2EnvelopeVolume = Sound2EnvelopeVolumeInitial;
            }
            break;
            
        //Sound Channel 3 - Sound On / Off
        case 0xFF1A:
            Sound3On = value>>7;
            break;
            
        //Sound Channel 3 - Sound Length
        case 0xFF1B:
            Sound3SoundLength = (256-value)*500;
            break;
            
        //Sound Channel 3 - Select Output Level - Crude Volume Control
        case 0xFF1C:
            switch ((value>>5)&3) //Mute, Normal, 1 Shift Right, 2 Shift Right
            {
                case 0:
                    Sound3OutputLevel = 4; //Right Shift All Off
                    break;
                    
                case 1:
                    Sound3OutputLevel = 0;
                    break;
                    
                case 2:
                    Sound3OutputLevel = 1;
                    break;
                    
                case 3:
                    Sound3OutputLevel = 2;
                    break;
            }
            break;
            
        //Sound Channel 3 - Frequency Low
        case 0xFF1D:
            Sound3Frequency = (Sound3Frequency&1792)|value;
            Sound3FrequencyReal = 131072.0/(double)(2048-Sound3Frequency);
            break;
            
        //Sound Channel 3 - Reset Sound - Counter Select - Frequency High
        case 0xFF1E:
            Sound3Restart = value>>7;
            Sound3CounterSelect = (value>>6)&1;
            Sound3Frequency = (Sound3Frequency&255)|(((int)(value&7))<<8);
            Sound3FrequencyReal = 131072.0/(double)(2048-Sound3Frequency);
            
            if (Sound3Restart)
            {
                Sound3SoundLengthStop = soundTick + Sound3SoundLength;
            }
            break;
            
        //Sound Channel 4 - Sound Length
        case 0xFF20:
            Sound4SoundLength = (64-(value&63))*4;
            break;
            
        //Sound Channel 4 - Envelope
        case 0xFF21:
            Sound4EnvelopeVolumeInitial = (value>>4)&7;
            if (Sound4EnvelopeVolumeInitial == 0) //Zero mutes immediately otherwise wait till sound reset
            {
                Sound4EnvelopeVolume = 0;
            }
            Sound4EnvelopeDirection = (value>>3)&1;
            Sound4EnvelopeSweepsLeft = value&7;
            Sound4NextEnvelopeVolumeShift = soundTick + 15;
            break;
            
        //Sound Channel 4 - Polynomial Counter - ???
        case 0xFF22:
            Sound4ShiftClockFrequency = value>>4;
            Sound4LFSRSize = (value>>3)&1;
            Sound4LFSRDividingRatio = value&7;
            break;
            
        //Sound Channel 4 - Counter Select
        case 0xFF23:
            Sound4Restart = value>>7;
            Sound4CounterSelect = (value>>6)&1;
            
            if (Sound4Restart)
            {
                Sound4SoundLengthStop = soundTick + Sound4SoundLength;
            }
            break;
            
        //Sound Channel Volume Control
        case 0xFF24:
            SoundChannel1Volume = (value&7)*0.125;
            SoundChannel2Volume = ((value>>4)&7)*0.125;
            evaluateSoundLevels();
            
            //N.B. There is also a command here for connecting a Mic to the sound terminal!
            //This is not implemented because I've never seen a gameboy game that uses a mic!
            break;
        
        //Sound Channel Connect Channels To Volumes
        case 0xFF25:
            SoundConnections = value;
            evaluateSoundLevels();
            break;
            
        //Sound Master Switch
        case 0xFF26:
            MasterSoundSwitch = value>>7;
            break;
            
        default:
            break;
    }
}

void Sound::evaluateSoundLevels()
{
    //Evaluate New Sound Levels For All Channels
    Sound1VolumeScale = 0.0;
    Sound2VolumeScale = 0.0;
    Sound3VolumeScale = 0.0;
    Sound4VolumeScale = 0.0;
    
    if (SoundConnections&1)
    {
        Sound1VolumeScale = SoundChannel1Volume;
    }
    if (SoundConnections&2)
    {
        Sound2VolumeScale = SoundChannel1Volume;
    }
    if (SoundConnections&4)
    {
        Sound3VolumeScale = SoundChannel1Volume;
    }
    if (SoundConnections&8)
    {
        Sound4VolumeScale = SoundChannel1Volume;
    }
    if (SoundConnections&16)
    {
        Sound1VolumeScale = SoundChannel2Volume;
    }
    if (SoundConnections&32)
    {
        Sound2VolumeScale = SoundChannel2Volume;
    }
    if (SoundConnections&64)
    {
        Sound3VolumeScale = SoundChannel2Volume;
    }
    if (SoundConnections&128)
    {
        Sound4VolumeScale = SoundChannel2Volume;
    }
}

unsigned char Sound::requestValue(unsigned int address, unsigned int lastValue)
{
    switch (address)
    {
        case 0xFF10:
        case 0xFF12:
        case 0xFF16:
        case 0xFF17:
        case 0xFF1B:
        case 0xFF20:
        case 0xFF21:
        case 0xFF22:
        case 0xFF24:
        case 0xFF25:
            return lastValue;
            break;
            
        case 0xFF11:
            return lastValue|63; //7th and 6th Bit Only
            break;
            
        case 0xFF14:
        case 0xFF19:
        case 0xFF1E:
        case 0xFF23:
            return lastValue|191; //6th Bit only
            break;
            
        case 0xFF1C:
            return lastValue|159; //6th and 5th Bit
            break;
            
        case 0xFF1A:
            return lastValue|127; //7th Bit Only
            break;
            
        case 0xFF26: //Get status of all sound channels
            return Sound1InternalOn+2*Sound2InternalOn+4*Sound3InternalOn+8*Sound4InternalOn+16+32+64+128*MasterSoundSwitch;
            break;
            
        default:
            return 0xFF;
            break;
    }
}

#endif
