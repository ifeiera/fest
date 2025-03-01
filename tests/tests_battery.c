#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_battery_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"battery\"") != NULL);

    /* Validate important Battery fields
     *  check if is_desktop are present
     *  check if percent are present
     *  check if power_plugged are present
     */
    assert(strstr(jsonData, "\"is_desktop\"") != NULL);
    assert(strstr(jsonData, "\"percent\"") != NULL);
    assert(strstr(jsonData, "\"power_plugged\"") != NULL);

    printf("Battery test passed\n");
}

int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_battery_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
