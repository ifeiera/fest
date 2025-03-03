#ifndef AUDIO_INFO_H
#define AUDIO_INFO_H

#include <windows.h>

/**
 * @brief Information about a single audio device
 *
 * Contains identification details for an audio device:
 * - Device name/description
 * - Manufacturer information
 *
 * @note Maximum string length is 255 characters
 */
typedef struct
{
    char name[256];         // Device name/description
    char manufacturer[256]; // Device manufacturer
} AudioDeviceInfo;

/**
 * @brief Container for multiple audio devices
 *
 * Holds a dynamically allocated array of audio devices and count:
 * - Array of AudioDeviceInfo structures
 * - Number of devices in the array
 *
 * @note Caller must free using freeAudioList()
 */
typedef struct
{
    AudioDeviceInfo *devices; // Array of audio devices
    UINT count;               // Number of devices
} AudioList;

/**
 * @brief Retrieves information about installed audio devices
 *
 * This function:
 * 1. Enumerates system audio devices
 * 2. Collects device details
 * 3. Allocates and fills AudioList structure
 *
 * @return AudioList* Pointer to allocated audio device list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeAudioList()
 */
AudioList *getAudioList(void);

/**
 * @brief Frees memory allocated for audio device list
 *
 * @param list Pointer to AudioList structure to be freed
 */
void freeAudioList(AudioList *list);

/**
 * @brief Gets the name/description of an audio device
 *
 * @param device Pointer to AudioDeviceInfo structure
 * @return const char* Device name or empty string if device is NULL
 */
const char *getAudioDeviceName(const AudioDeviceInfo *device);

/**
 * @brief Gets the manufacturer of an audio device
 *
 * @param device Pointer to AudioDeviceInfo structure
 * @return const char* Manufacturer name or empty string if device is NULL
 */
const char *getAudioDeviceManufacturer(const AudioDeviceInfo *device);

#endif // AUDIO_INFO_H
