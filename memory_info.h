#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <windows.h>

// Struktur untuk menyimpan informasi slot RAM
typedef struct
{
    UINT64 capacity;      // dalam bytes
    UINT speed;           // dalam MHz
    UINT configuredSpeed; // Tambahkan field baru untuk configured speed
    char slot[64];        // lokasi slot
    char manufacturer[256];
} RAMSlotInfo;

// Struktur untuk menyimpan array dari slot RAM
typedef struct
{
    RAMSlotInfo *slots;
    UINT count;
} RAMSlotList;

// Struktur untuk menyimpan informasi memori sistem
typedef struct
{
    UINT64 totalPhys;     // Total RAM fisik (bytes)
    UINT64 availPhys;     // RAM tersedia (bytes)
    UINT64 usedPhys;      // RAM terpakai (bytes)
    DWORD memoryLoad;     // Persentase penggunaan (0-100)
    RAMSlotList slotList; // Informasi slot RAM
} MemoryInfo;

// Fungsi-fungsi untuk mendapatkan informasi memori
MemoryInfo *getMemoryInfo(void);
void freeMemoryInfo(MemoryInfo *info);

// Fungsi helper untuk konversi
double bytesToGB(UINT64 bytes);
const char *getRAMSlotLocation(const RAMSlotInfo *slot);
const char *getRAMManufacturer(const RAMSlotInfo *slot);
UINT64 getRAMCapacity(const RAMSlotInfo *slot);
UINT getRAMSpeed(const RAMSlotInfo *slot);

// Tambahkan deklarasi fungsi getter
UINT getRAMConfiguredSpeed(const RAMSlotInfo *slot);

#endif // MEMORY_INFO_H
