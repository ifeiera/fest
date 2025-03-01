#include "system_info_dll.h"
#include <stdio.h>

// Callback function for receiving JSON data
void onSystemInfoUpdate(const char *jsonData)
{
    printf("%s\n", jsonData);
    printf("----------------------------------------\n");
}

int main()
{
    // Set callback for receiving data
    setSystemInfoCallback(onSystemInfoUpdate);

    // Start monitoring with 2 second interval
    if (!startSystemMonitoring(500))
    {
        printf("Failed to start system monitoring!\n");
        return 1;
    }

    printf("System monitoring started. Press Enter to stop...\n");
    getchar();

    // Stop monitoring
    stopSystemMonitoring();

    return 0;
}
