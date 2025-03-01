#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_motherboard_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"motherboard\"") != NULL);

    // Validate important Motherboard fields
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);
    assert(strstr(jsonData, "\"product\"") != NULL);
    assert(strstr(jsonData, "\"serial_number\"") != NULL);
    assert(strstr(jsonData, "\"bios_version\"") != NULL);

    printf("Motherboard test passed\n");
}

int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_motherboard_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
