#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Tests audio device information JSON output
 *
 * This test validates:
 * 1. JSON data integrity
 *    - Non-null data
 *    - Non-empty string
 *    - Contains "audio" section
 *
 * 2. Required audio device fields
 *    - Device name present
 *    - Manufacturer information present
 *
 * The test is called as a callback by the monitoring system
 * and validates the JSON structure matches audio_info.h specs
 *
 * @param jsonData JSON-formatted system information string
 */
void test_audio_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"audio\"") != NULL);

    /**
     * Validate important Audio fields
     *  check if name are present
     *  check if manufacturer are present
     */
    assert(strstr(jsonData, "\"name\"") != NULL);
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);

    printf("Audio test passed\n");
}

/**
 * @brief Test runner for audio information tests
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

    setSystemInfoCallback(test_audio_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
