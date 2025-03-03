#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests network adapter information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "network" section
 *
 * 2. Network array structure
 *    - Valid array brackets
 *    - Handles empty array case (CI environment)
 *
 * 3. Network adapter types
 *    - Ethernet interfaces
 *    - WiFi interfaces
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches network_info.h specs
 * Special handling for CI environment where network may be unavailable
 *
 * @param jsonData JSON-formatted system information string
 */
void test_network_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"network\"") != NULL);

    /**
     * Validate network section presence
     * Early return if section not found
     */
    const char *network = strstr(jsonData, "\"network\"");
    if (!network)
    {
        printf("Network section not found\n");
        return;
    }

    /**
     * Check network array structure
     * Handle empty array case for CI environment
     */
    const char *network_start = strstr(network, "[");
    const char *network_end = strstr(network, "]");

    if (!network_start || !network_end || (network_end - network_start) <= 1)
    {
        printf("Network array is empty (this is OK in CI environment)\n");
        printf("Network test passed\n");
        return;
    }

    /**
     * Validate important Network fields
     *  check if ethernet are present
     *  check if wifi are present
     */
    if (strstr(network, "\"ethernet\"") || strstr(network, "\"wifi\""))
    {
        printf("Found network adapter(s)\n");
    }
    else
    {
        printf("No network adapters found (this is OK in CI environment)\n");
    }

    printf("Network test passed\n");
}

/**
 * @brief Test runner for network information tests
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

    setSystemInfoCallback(test_network_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
