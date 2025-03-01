#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <windows.h>
#include <dxgi.h>
#include <stdio.h>
#include <initguid.h>

// Structure for storing GPU information
typedef struct
{
    char name[128];
    UINT64 dedicatedMemory; // in bytes
    UINT64 sharedMemory;    // in bytes
    int isIntegrated;       // 1 if iGPU, 0 if dGPU
    int adapterIndex;       // adapter index
} GPUInfo;

// Structure for storing array of GPUs
typedef struct
{
    GPUInfo *gpus;
    UINT count;
} GPUList;

// Functions for getting GPU information
GPUList *getGPUList(void);
void freeGPUList(GPUList *list);
void printGPUInfo(const GPUInfo *gpu);
const char *getGPUName(const GPUInfo *gpu);
UINT64 getGPUDedicatedMemory(const GPUInfo *gpu);
UINT64 getGPUSharedMemory(const GPUInfo *gpu);
int isIntegratedGPU(const GPUInfo *gpu);

#endif // GPU_INFO_H
