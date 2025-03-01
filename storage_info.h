#ifndef STORAGE_INFO_H
#define STORAGE_INFO_H

#include <windows.h>

// Struktur untuk menyimpan informasi disk fisik
typedef struct
{
    char model[256];
    char interfaceType[64];
    char deviceID[256];
} PhysicalDiskInfo;

// Struktur untuk menyimpan informasi disk logis
typedef struct
{
    char drive[4];          // Drive letter (C:, D:, etc.)
    char type[32];          // Local Disk atau Removable Disk
    char model[256];        // Model disk atau nama volume
    char interfaceType[64]; // Ubah dari interface ke interfaceType
    double totalSize;       // Ukuran total dalam GB
    double freeSpace;       // Ruang kosong dalam GB
} LogicalDiskInfo;

// Struktur untuk menyimpan array dari disk
typedef struct
{
    LogicalDiskInfo *disks;
    UINT count;
} StorageList;

// Fungsi-fungsi untuk mendapatkan informasi storage
StorageList *getStorageList(void);
void freeStorageList(StorageList *list);

// Fungsi getter
const char *getDiskDrive(const LogicalDiskInfo *disk);
const char *getDiskType(const LogicalDiskInfo *disk);
const char *getDiskModel(const LogicalDiskInfo *disk);
const char *getDiskInterface(const LogicalDiskInfo *disk);
double getDiskTotalSize(const LogicalDiskInfo *disk);
double getDiskFreeSpace(const LogicalDiskInfo *disk);

#endif // STORAGE_INFO_H
