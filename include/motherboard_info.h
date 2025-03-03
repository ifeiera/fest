#ifndef MOTHERBOARD_INFO_H
#define MOTHERBOARD_INFO_H

#include <windows.h>

/**
 * @brief Comprehensive motherboard and BIOS information
 *
 * Contains detailed system board specifications:
 * - Product identification
 * - Manufacturing details
 * - BIOS configuration
 * - System tracking information
 *
 * All fields have a maximum length of 255 characters
 * Empty fields are represented by empty strings
 */
typedef struct
{
    char productName[256];  // Motherboard model name
    char manufacturer[256]; // Board manufacturer
    char serialNumber[256]; // Board serial number
    char biosVersion[256];  // BIOS version string
    char biosSerial[256];   // BIOS serial number
    char systemSKU[256];    // System SKU identifier
} MotherboardInfo;

/**
 * @brief Retrieves motherboard and BIOS information
 *
 * This function:
 * 1. Queries WMI for system board data
 * 2. Collects BIOS information
 * 3. Retrieves system identifiers
 *
 * @return MotherboardInfo* Pointer to allocated information, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeMotherboardInfo()
 */
MotherboardInfo *getMotherboardInfo(void);

/**
 * @brief Frees memory allocated for motherboard information
 *
 * @param info Pointer to MotherboardInfo structure to be freed
 */
void freeMotherboardInfo(MotherboardInfo *info);

/**
 * @brief Gets the motherboard product name/model
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Product name or empty string if info is NULL
 */
const char *getMotherboardProduct(const MotherboardInfo *info);

/**
 * @brief Gets the motherboard manufacturer
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Manufacturer name or empty string if info is NULL
 */
const char *getMotherboardManufacturer(const MotherboardInfo *info);

/**
 * @brief Gets the motherboard serial number
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Serial number or empty string if info is NULL
 */
const char *getMotherboardSerial(const MotherboardInfo *info);

/**
 * @brief Gets the BIOS version string
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* BIOS version or empty string if info is NULL
 */
const char *getMotherboardBiosVersion(const MotherboardInfo *info);

/**
 * @brief Gets the BIOS serial number
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* BIOS serial or empty string if info is NULL
 */
const char *getMotherboardBiosSerial(const MotherboardInfo *info);

/**
 * @brief Gets the system SKU identifier
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* System SKU or empty string if info is NULL
 */
const char *getMotherboardSystemSKU(const MotherboardInfo *info);

#endif // MOTHERBOARD_INFO_H
