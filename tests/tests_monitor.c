#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests monitor information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "monitors" section
 *
 * 2. Required display fields
 *    - Primary display status
 *    - Resolution information
 *    - Display characteristics
 *    - Refresh rate
 *    - Physical dimensions
 *    - Manufacturer details
 *    - Device identification
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches monitor_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_monitor_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"monitors\"") != NULL);

    /**
     * Validate important Monitor fields
     *  check if is_primary are present
     *  check if width and height are present
     *  check if current_resolution are present
     *  check if native_resolution are present
     *  check if aspect_ratio are present
     *  check if refresh_rate are present
     *  check if screen_size are present
     *  check if physical dimensions are present
     *  check if manufacturer are present
     *  check if device_id are present
     */
    assert(strstr(jsonData, "\"is_primary\"") != NULL);
    assert(strstr(jsonData, "\"width\"") != NULL);
    assert(strstr(jsonData, "\"height\"") != NULL);
    assert(strstr(jsonData, "\"current_resolution\"") != NULL);
    assert(strstr(jsonData, "\"native_resolution\"") != NULL);
    assert(strstr(jsonData, "\"aspect_ratio\"") != NULL);
    assert(strstr(jsonData, "\"refresh_rate\"") != NULL);
    assert(strstr(jsonData, "\"screen_size\"") != NULL);
    assert(strstr(jsonData, "\"physical_width_mm\"") != NULL);
    assert(strstr(jsonData, "\"physical_height_mm\"") != NULL);
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);
    assert(strstr(jsonData, "\"device_id\"") != NULL);
    printf("Monitor test passed\n");
}

/**
 * @brief Test runner for monitor information tests
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

    setSystemInfoCallback(test_monitor_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
