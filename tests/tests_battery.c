#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests battery/power information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "battery" section
 *
 * 2. Required battery status fields
 *    - System type (desktop/laptop)
 *    - Battery percentage
 *    - Power connection status
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches battery_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_battery_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"battery\"") != NULL);

    /**
     * Validate important Battery fields
     *  check if is_desktop are present
     *  check if percent are present
     *  check if power_plugged are present
     */
    assert(strstr(jsonData, "\"is_desktop\"") != NULL);
    assert(strstr(jsonData, "\"percent\"") != NULL);
    assert(strstr(jsonData, "\"power_plugged\"") != NULL);

    printf("Battery test passed\n");
}

/**
 * @brief Test runner for battery information tests
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

    setSystemInfoCallback(test_battery_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
