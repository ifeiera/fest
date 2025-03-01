#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <windows.h>

// Structure for storing RAM slot information
typedef struct
{
    UINT64 capacity;        // in bytes
    UINT speed;             // in MHz
    UINT configuredSpeed;   // Add new field for configured speed
    char slot[64];          // slot location
    char manufacturer[256]; // manufacturer
} RAMSlotInfo;

// Structure for storing array of RAM slots
typedef struct
{
    RAMSlotInfo *slots;
    UINT count;
} RAMSlotList;

// Structure for storing system memory information
typedef struct
{
    UINT64 totalPhys;     // Total physical RAM (bytes)
    UINT64 availPhys;     // Available RAM (bytes)
    UINT64 usedPhys;      // Used RAM (bytes)
    DWORD memoryLoad;     // Memory usage percentage (0-100)
    RAMSlotList slotList; // RAM slot information
} MemoryInfo;

// Functions for getting memory information
MemoryInfo *getMemoryInfo(void);
void freeMemoryInfo(MemoryInfo *info);

// Helper function for converting bytes to GB
double bytesToGB(UINT64 bytes);
const char *getRAMSlotLocation(const RAMSlotInfo *slot);
const char *getRAMManufacturer(const RAMSlotInfo *slot);
UINT64 getRAMCapacity(const RAMSlotInfo *slot);
UINT getRAMSpeed(const RAMSlotInfo *slot);

// Add declaration for getter functions
UINT getRAMConfiguredSpeed(const RAMSlotInfo *slot);

#endif // MEMORY_INFO_H
