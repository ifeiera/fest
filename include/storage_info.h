#ifndef STORAGE_INFO_H
#define STORAGE_INFO_H

#include <windows.h>

// Structure for storing physical disk information
typedef struct
{
    char model[256];
    char interfaceType[64];
    char deviceID[256];
} PhysicalDiskInfo;

// Structure for storing logical disk information
typedef struct
{
    char drive[4];          // Drive letter (C:, D:, etc.)
    char type[32];          // Local Disk or Removable Disk
    char model[256];        // Disk model or volume name
    char interfaceType[64]; // Change from interface to interfaceType
    double totalSize;       // Total size in GB
    double freeSpace;       // Free space in GB
} LogicalDiskInfo;

// Structure for storing array of disks
typedef struct
{
    LogicalDiskInfo *disks;
    UINT count;
} StorageList;

// Functions for getting storage information
StorageList *getStorageList(void);
void freeStorageList(StorageList *list);

// Getter functions
const char *getDiskDrive(const LogicalDiskInfo *disk);
const char *getDiskType(const LogicalDiskInfo *disk);
const char *getDiskModel(const LogicalDiskInfo *disk);
const char *getDiskInterface(const LogicalDiskInfo *disk);
double getDiskTotalSize(const LogicalDiskInfo *disk);
double getDiskFreeSpace(const LogicalDiskInfo *disk);

#endif // STORAGE_INFO_H
