#ifndef AUDIO_INFO_H
#define AUDIO_INFO_H

#include <windows.h>

// Struktur untuk menyimpan informasi perangkat audio
typedef struct
{
    char name[256];
    char manufacturer[256];
} AudioDeviceInfo;

// Struktur untuk menyimpan array dari perangkat audio
typedef struct
{
    AudioDeviceInfo *devices;
    UINT count;
} AudioList;

// Fungsi-fungsi untuk mendapatkan informasi audio
AudioList *getAudioList(void);
void freeAudioList(AudioList *list);

// Fungsi getter
const char *getAudioDeviceName(const AudioDeviceInfo *device);
const char *getAudioDeviceManufacturer(const AudioDeviceInfo *device);

#endif // AUDIO_INFO_H
