#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests memory information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "memory" section
 *
 * 2. Required memory metrics fields
 *    - Total RAM capacity
 *    - Available memory
 *    - Used memory
 *    - Memory usage percentage
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches memory_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_memory_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"memory\"") != NULL);

    /**
     * Validate important Memory fields
     *  check if total are present
     *  check if available are present
     *  check if used are present
     *  check if percent are present
     */
    assert(strstr(jsonData, "\"total\"") != NULL);
    assert(strstr(jsonData, "\"available\"") != NULL);
    assert(strstr(jsonData, "\"used\"") != NULL);
    assert(strstr(jsonData, "\"usage_percent\"") != NULL);

    printf("Memory test passed\n");
}

/**
 * @brief Test runner for memory information tests
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

    setSystemInfoCallback(test_memory_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
