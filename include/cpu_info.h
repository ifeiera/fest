#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <windows.h>

// Structure for storing CPU information
typedef struct
{
    char name[256];
    UINT cores;
    UINT threads;
    UINT clockSpeed;
} CPUInfo;

// Structure for storing array of CPUs
typedef struct
{
    CPUInfo *cpus;
    UINT count;
} CPUList;

// Functions for getting CPU information
CPUList *getCPUList(void);
void freeCPUList(CPUList *list);

// Getter functions
const char *getCPUName(const CPUInfo *cpu);
UINT getCPUCores(const CPUInfo *cpu);
UINT getCPUThreads(const CPUInfo *cpu);
UINT getCPUClockSpeed(const CPUInfo *cpu);

#endif // CPU_INFO_H
