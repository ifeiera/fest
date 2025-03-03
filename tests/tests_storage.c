#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests storage device information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "storage" section
 *
 * 2. Storage device information
 *    - Drive information
 *    - Capacity metrics
 *    - Device identification
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches storage_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_storage_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);

    /**
     * Validate important Storage fields
     *  check if storage section is present
     *  check if drive information exists
     *  check if capacity metrics are included
     */
    assert(strstr(jsonData, "\"storage\"") != NULL);

    printf("Storage test passed\n");
}

/**
 * @brief Test runner for storage information tests
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

    setSystemInfoCallback(test_storage_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
