#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_monitor_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"monitors\"") != NULL);

    // Validate important Monitor fields
    assert(strstr(jsonData, "\"is_primary\"") != NULL);
    assert(strstr(jsonData, "\"current_resolution\"") != NULL);
    assert(strstr(jsonData, "\"aspect_ratio\"") != NULL);
    assert(strstr(jsonData, "\"refresh_rate\"") != NULL);
    assert(strstr(jsonData, "\"screen_size\"") != NULL);
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);
    assert(strstr(jsonData, "\"device_id\"") != NULL);

    printf("Monitor test passed\n");
}

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
