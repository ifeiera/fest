#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_memory_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"memory\"") != NULL);

    /* Validate important Memory fields
     *  check if total are present
     *  check if available are present
     *  check if used are present
     *  check if percent are present
     */
    assert(strstr(jsonData, "\"total\"") != NULL);
    assert(strstr(jsonData, "\"available\"") != NULL);
    assert(strstr(jsonData, "\"used\"") != NULL);
    assert(strstr(jsonData, "\"usage_percent\"") != NULL);

    printf("Memory test passed\n");
}

int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_memory_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
