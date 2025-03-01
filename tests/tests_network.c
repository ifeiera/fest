#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_network_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"network\"") != NULL);

    // Validate important Network fields
    assert(strstr(jsonData, "\"ethernet\"") != NULL || strstr(jsonData, "\"wifi\"") != NULL);
    assert(strstr(jsonData, "\"name\"") != NULL);
    assert(strstr(jsonData, "\"mac_address\"") != NULL);

    printf("Network test passed\n");
}

int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_network_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
