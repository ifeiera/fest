#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <windows.h>
#include <dxgi.h>
#include <stdio.h>
#include <initguid.h>

/**
 * @brief Information about a single graphics processing unit
 *
 * Contains detailed GPU specifications:
 * - Device name and type
 * - Memory configuration
 * - Adapter details
 *
 * Memory values are reported in bytes for maximum precision
 *
 * @note Maximum GPU name length is 127 characters
 */
typedef struct
{
    char name[128];         // GPU name/description
    double dedicatedMemory; // Dedicated video memory in GB
    double sharedMemory;    // Shared system memory in GB
    BOOL isIntegrated;      // Integrated vs Discrete GPU flag
    UINT adapterIndex;      // Adapter enumeration index
} GPUInfo;

/**
 * @brief Container for multiple GPU information
 *
 * Holds information about all graphics adapters:
 * - Array of GPUInfo structures
 * - Number of adapters
 *
 * Systems may have multiple GPUs (e.g. integrated + discrete)
 *
 * @note Caller must free using freeGPUList()
 */
typedef struct
{
    GPUInfo *gpus; // Array of GPU information
    UINT count;    // Number of GPUs
} GPUList;

/**
 * @brief Retrieves information about system graphics adapters
 *
 * This function:
 * 1. Enumerates DXGI adapters
 * 2. Collects detailed specifications
 * 3. Allocates and fills GPUList structure
 *
 * @return GPUList* Pointer to allocated GPU list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeGPUList()
 */
GPUList *getGPUList(void);

/**
 * @brief Frees memory allocated for GPU information list
 *
 * @param list Pointer to GPUList structure to be freed
 */
void freeGPUList(GPUList *list);

/**
 * @brief Prints detailed GPU information to stdout
 *
 * Outputs:
 * - Device name
 * - Memory configuration
 * - Adapter type and index
 *
 * @param gpu Pointer to GPUInfo structure to print
 */
void printGPUInfo(const GPUInfo *gpu);

/**
 * @brief Gets the name of a graphics adapter
 *
 * @param gpu Pointer to GPUInfo structure
 * @return const char* GPU name or empty string if gpu is NULL
 */
const char *getGPUName(const GPUInfo *gpu);

/**
 * @brief Gets the amount of dedicated video memory
 *
 * @param gpu Pointer to GPUInfo structure
 * @return double Dedicated video memory in GB or 0.0 if gpu is NULL
 */
double getGPUDedicatedMemory(const GPUInfo *gpu);

/**
 * @brief Gets the amount of shared system memory
 *
 * @param gpu Pointer to GPUInfo structure
 * @return double Shared system memory in GB or 0.0 if gpu is NULL
 */
double getGPUSharedMemory(const GPUInfo *gpu);

/**
 * @brief Checks if the GPU is an integrated adapter
 *
 * @param gpu Pointer to GPUInfo structure
 * @return int 1 if integrated GPU, 0 if discrete or gpu is NULL
 */
int isIntegratedGPU(const GPUInfo *gpu);

#endif // GPU_INFO_H
