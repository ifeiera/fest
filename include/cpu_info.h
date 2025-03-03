#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <windows.h>

/**
 * @brief Information about a single CPU/processor
 *
 * Contains detailed processor specifications:
 * - Full processor name/model
 * - Physical core count
 * - Logical thread count
 * - Base clock frequency
 *
 * @note Maximum processor name length is 255 characters
 */
typedef struct
{
    char name[256];  // Full processor name/model
    UINT cores;      // Number of physical cores
    UINT threads;    // Number of logical threads
    UINT clockSpeed; // Base clock speed in MHz
} CPUInfo;

/**
 * @brief Container for multiple CPU information
 *
 * Holds information about all processors in the system:
 * - Array of CPUInfo structures
 * - Number of processors
 *
 * For multi-socket systems, count may be > 1
 *
 * @note Caller must free using freeCPUList()
 */
typedef struct
{
    CPUInfo *cpus; // Array of processor information
    UINT count;    // Number of processors
} CPUList;

/**
 * @brief Retrieves information about system processors
 *
 * This function:
 * 1. Enumerates physical processors
 * 2. Collects detailed specifications
 * 3. Allocates and fills CPUList structure
 *
 * @return CPUList* Pointer to allocated processor list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeCPUList()
 */
CPUList *getCPUList(void);

/**
 * @brief Frees memory allocated for CPU information list
 *
 * @param list Pointer to CPUList structure to be freed
 */
void freeCPUList(CPUList *list);

/**
 * @brief Gets the name/model of a processor
 *
 * @param cpu Pointer to CPUInfo structure
 * @return const char* Processor name or empty string if cpu is NULL
 */
const char *getCPUName(const CPUInfo *cpu);

/**
 * @brief Gets the number of physical cores
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Number of cores or 0 if cpu is NULL
 */
UINT getCPUCores(const CPUInfo *cpu);

/**
 * @brief Gets the number of logical threads
 *
 * For processors with hyperthreading/SMT,
 * this will be greater than the core count
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Number of threads or 0 if cpu is NULL
 */
UINT getCPUThreads(const CPUInfo *cpu);

/**
 * @brief Gets the base clock frequency
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Clock speed in MHz or 0 if cpu is NULL
 */
UINT getCPUClockSpeed(const CPUInfo *cpu);

#endif // CPU_INFO_H
