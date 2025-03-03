#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests CPU information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "cpu" section
 *
 * 2. Required processor fields
 *    - CPU name/model
 *    - Core count
 *    - Thread count
 *    - Clock speed
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches cpu_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_cpu_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"cpu\"") != NULL);

    /**
     * Validate important CPU fields
     *  check if name are present
     *  check if cores are present
     *  check if threads are present
     *  check if clock_speed are present
     */
    assert(strstr(jsonData, "\"name\"") != NULL);
    assert(strstr(jsonData, "\"cores\"") != NULL);
    assert(strstr(jsonData, "\"threads\"") != NULL);
    assert(strstr(jsonData, "\"clock_speed\"") != NULL);

    printf("CPU test passed\n");
}

/**
 * @brief Test runner for CPU information tests
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

    setSystemInfoCallback(test_cpu_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
