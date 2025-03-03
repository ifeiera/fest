#include "system_info_dll.h"
#include <stdio.h>

/**
 * @brief Callback function for receiving system information updates
 *
 * This function is called by the monitoring system whenever new
 * system information is available. It:
 * 1. Prints the received JSON data
 * 2. Adds a visual separator for readability
 *
 * @param jsonData JSON-formatted system information string
 */
void onSystemInfoUpdate(const char *jsonData)
{
    printf("%s\n", jsonData);
    printf("---------------------------------------\n");
}

/**
 * @brief Test application entry point
 *
 * This application demonstrates the usage of the system monitoring DLL:
 * 1. Sets up callback for receiving updates
 * 2. Starts monitoring with 500ms interval
 * 3. Waits for user input to stop
 * 4. Performs cleanup
 *
 * @return int 0 on success, 1 on monitoring start failure
 */
int main()
{
    // Register callback for system information updates
    setSystemInfoCallback(onSystemInfoUpdate);

    // Start monitoring with 500ms update interval
    if (!startSystemMonitoring(500))
    {
        printf("Failed to start system monitoring!\n");
        return 1;
    }

    printf("System monitoring started. Press Enter to stop...\n");
    getchar();

    // Stop monitoring and cleanup
    stopSystemMonitoring();

    return 0;
}
