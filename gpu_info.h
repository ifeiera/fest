#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <windows.h>
#include <dxgi.h>
#include <stdio.h>
#include <initguid.h>

// Struktur untuk menyimpan informasi GPU
typedef struct
{
    char name[128];
    UINT64 dedicatedMemory; // dalam bytes
    UINT64 sharedMemory;    // dalam bytes
    int isIntegrated;       // 1 jika iGPU, 0 jika dGPU
    int adapterIndex;
} GPUInfo;

// Struktur untuk menyimpan array dari GPU
typedef struct
{
    GPUInfo *gpus;
    UINT count;
} GPUList;

// Fungsi-fungsi untuk mendapatkan informasi GPU
GPUList *getGPUList(void);
void freeGPUList(GPUList *list);
void printGPUInfo(const GPUInfo *gpu);
const char *getGPUName(const GPUInfo *gpu);
UINT64 getGPUDedicatedMemory(const GPUInfo *gpu);
UINT64 getGPUSharedMemory(const GPUInfo *gpu);
int isIntegratedGPU(const GPUInfo *gpu);

#endif // GPU_INFO_H
