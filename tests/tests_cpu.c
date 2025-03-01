#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_cpu_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"cpu\"") != NULL);

    // Validate important CPU fields
    assert(strstr(jsonData, "\"name\"") != NULL);
    assert(strstr(jsonData, "\"cores\"") != NULL);
    assert(strstr(jsonData, "\"threads\"") != NULL);
    assert(strstr(jsonData, "\"clock_speed\"") != NULL);

    printf("CPU test passed\n");
}

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
