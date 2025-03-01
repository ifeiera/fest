#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void print_final_json(const char *jsonData)
{
    printf("\n--- Final System Information JSON ---\n");
    printf("%s\n", jsonData);
    printf("--- End of System Information JSON ---\n");
}

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
