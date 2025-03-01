#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <windows.h>

// Struktur untuk menyimpan informasi CPU
typedef struct
{
    char name[256];
    UINT cores;
    UINT threads;
    UINT clockSpeed;
} CPUInfo;

// Struktur untuk menyimpan array dari CPU
typedef struct
{
    CPUInfo *cpus;
    UINT count;
} CPUList;

// Fungsi-fungsi untuk mendapatkan informasi CPU
CPUList *getCPUList(void);
void freeCPUList(CPUList *list);

// Fungsi getter
const char *getCPUName(const CPUInfo *cpu);
UINT getCPUCores(const CPUInfo *cpu);
UINT getCPUThreads(const CPUInfo *cpu);
UINT getCPUClockSpeed(const CPUInfo *cpu);

#endif // CPU_INFO_H
