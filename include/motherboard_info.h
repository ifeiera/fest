#ifndef MOTHERBOARD_INFO_H
#define MOTHERBOARD_INFO_H

#include <windows.h>

// Structure for storing motherboard information
typedef struct
{
    char productName[256];
    char manufacturer[256];
    char serialNumber[256];
    char biosVersion[256];
    char biosSerial[256];
    char systemSKU[256];
} MotherboardInfo;

// Functions for getting motherboard information
MotherboardInfo *getMotherboardInfo(void);
void freeMotherboardInfo(MotherboardInfo *info);
const char *getMotherboardProduct(const MotherboardInfo *info);
const char *getMotherboardManufacturer(const MotherboardInfo *info);
const char *getMotherboardSerial(const MotherboardInfo *info);
const char *getMotherboardBiosVersion(const MotherboardInfo *info);
const char *getMotherboardBiosSerial(const MotherboardInfo *info);
const char *getMotherboardSystemSKU(const MotherboardInfo *info);

#endif // MOTHERBOARD_INFO_H
