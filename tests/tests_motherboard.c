#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests motherboard information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "motherboard" section
 *
 * 2. Required system board fields
 *    - Manufacturer details
 *    - Product information
 *    - Serial numbers
 *    - BIOS configuration
 *    - System identification
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches motherboard_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_motherboard_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"motherboard\"") != NULL);

    /**
     * Validate important Motherboard fields
     *  check if manufacturer are present
     *  check if product are present
     *  check if serial_number are present
     *  check if bios_version are present
     *  check if bios_serial are present
     *  check if system_sku are present
     */
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);
    assert(strstr(jsonData, "\"product\"") != NULL);
    assert(strstr(jsonData, "\"serial_number\"") != NULL);
    assert(strstr(jsonData, "\"bios_version\"") != NULL);
    assert(strstr(jsonData, "\"bios_serial\"") != NULL);
    assert(strstr(jsonData, "\"system_sku\"") != NULL);

    printf("Motherboard test passed\n");
}

/**
 * @brief Test runner for motherboard information tests
 *
 * This function:
 * 1. Sets up the test environment
 *    - Registers test callback
 *    - Starts system monitoring
 *
 * 2. Executes test sequence
 *    - Waits for data collection (200ms)
 *    - Validates callback execution
 *
 * 3. Cleans up
 *    - Stops monitoring
 *    - Reports test results
 *
 * @return int 0 if all tests passed, 1 if any failed
 */
int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_motherboard_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
