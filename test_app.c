#include "system_info_dll.h"
#include <stdio.h>

// Callback function untuk menerima data JSON
void onSystemInfoUpdate(const char *jsonData)
{
    printf("%s\n", jsonData);
    printf("----------------------------------------\n");
}

int main()
{
    // Set callback untuk menerima data
    setSystemInfoCallback(onSystemInfoUpdate);

    // Mulai monitoring dengan interval 2 detik
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
