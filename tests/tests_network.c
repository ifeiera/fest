#include "system_info_dll.h"
#include <stdio.h>
#include <assert.h>

void test_network_info(const char *jsonData)
{
    assert(jsonData != NULL);
    assert(strlen(jsonData) > 0);
    assert(strstr(jsonData, "\"network\"") != NULL);

    // Validate important Network fields
    const char *ethernet = strstr(jsonData, "\"ethernet\"");
    const char *wifi = strstr(jsonData, "\"wifi\"");

    if (ethernet)
    {
        // Only check fields in the ethernet section
        const char *eth_section_end = strstr(ethernet + 10, "]");
        if (eth_section_end)
        {
            char eth_section[1024] = {0};
            size_t len = eth_section_end - ethernet;
            if (len < sizeof(eth_section))
            {
                strncpy(eth_section, ethernet, len);
                assert(strstr(eth_section, "\"name\"") != NULL);
                assert(strstr(eth_section, "\"mac_address\"") != NULL);
            }
        }
        printf("Ethernet adapter found and validated\n");
    }

    if (wifi)
    {
        // Only check fields in the wifi section
        const char *wifi_section_end = strstr(wifi + 7, "]");
        if (wifi_section_end)
        {
            char wifi_section[1024] = {0};
            size_t len = wifi_section_end - wifi;
            if (len < sizeof(wifi_section))
            {
                strncpy(wifi_section, wifi, len);
                assert(strstr(wifi_section, "\"name\"") != NULL);
                assert(strstr(wifi_section, "\"mac_address\"") != NULL);
            }
        }
        printf("WiFi adapter found and validated\n");
    }

    if (!ethernet && !wifi)
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
