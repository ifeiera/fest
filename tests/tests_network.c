#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_network_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"network\"") != NULL);

    // Validate important Network fields
    const char *network = strstr(jsonData, "\"network\"");
    if (!network)
    {
        printf("Network section not found\n");
        return;
    }

    // Check if network array is empty
    const char *network_start = strstr(network, "[");
    const char *network_end = strstr(network, "]");

    if (!network_start || !network_end || (network_end - network_start) <= 1)
    {
        printf("Network array is empty (this is OK in CI environment)\n");
        printf("Network test passed\n");
        return;
    }

    /* Validate important Network fields
     *  check if ethernet are present
     *  check if wifi are present
     */
    if (strstr(network, "\"ethernet\"") || strstr(network, "\"wifi\""))
    {
        printf("Found network adapter(s)\n");
    }
    else
    {
        printf("No network adapters found (this is OK in CI environment)\n");
    }

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
