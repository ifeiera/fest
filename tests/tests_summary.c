#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

/**
 * @brief Prints complete system information in JSON format
 *
 * This function:
 * 1. Formats output with clear section markers
 * 2. Displays the complete JSON structure
 * 3. Helps verify overall JSON formatting
 *
 * Used for:
 * - Manual verification of JSON structure
 * - Debugging system information collection
 * - Validating complete system data
 *
 * @param jsonData Complete system information in JSON format
 */
void print_final_json(const char *jsonData)
{
    printf("\n--- Final System Information JSON ---\n");
    printf("%s\n", jsonData);
    printf("--- End of System Information JSON ---\n");
}

/**
 * @brief Summary test runner for system information
 *
 * This function:
 * 1. Sets up monitoring
 *    - Registers print callback
 *    - Starts system monitoring
 *
 * 2. Collects complete system information
 *    - Waits for data collection (200ms)
 *    - Displays formatted JSON output
 *
 * 3. Cleans up
 *    - Stops monitoring
 *    - Reports success/failure
 *
 * Used as final verification that all components
 * are working together correctly
 *
 * @return int 0 if monitoring completed, 1 if failed
 */
int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(print_final_json);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    return (testsPassed == totalTests) ? 0 : 1;
}
