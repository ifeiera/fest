#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_audio_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"audio\"") != NULL);

    /* Validate important Audio fields
     *  check if name are present
     *  check if manufacturer are present
     */
    assert(strstr(jsonData, "\"name\"") != NULL);
    assert(strstr(jsonData, "\"manufacturer\"") != NULL);

    printf("Audio test passed\n");
}

int main()
{
    int testsPassed = 0;
    int totalTests = 1;

    setSystemInfoCallback(test_audio_info);

    if (startSystemMonitoring(100))
    {
        Sleep(200);
        testsPassed++;
    }

    stopSystemMonitoring();

    printf("Tests passed: %d/%d\n", testsPassed, totalTests);
    return (testsPassed == totalTests) ? 0 : 1;
}
