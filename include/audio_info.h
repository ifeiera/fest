#ifndef AUDIO_INFO_H
#define AUDIO_INFO_H

#include <windows.h>

// Structure for storing audio device information
typedef struct
{
    char name[256];
    char manufacturer[256];
} AudioDeviceInfo;

// Structure for storing array of audio devices
typedef struct
{
    AudioDeviceInfo *devices;
    UINT count;
} AudioList;

// Functions for getting audio information
AudioList *getAudioList(void);
void freeAudioList(AudioList *list);

// Getter functions
const char *getAudioDeviceName(const AudioDeviceInfo *device);
const char *getAudioDeviceManufacturer(const AudioDeviceInfo *device);

#endif // AUDIO_INFO_H
