<img src="https://res.cloudinary.com/dzljlz2nn/image/upload/f_auto,q_auto/uploads/twu0lplckud1n3mtzgmt" alt="Banner" width="1024">

# FEST - Fast & Efficient System Tracking

[![Discord](https://img.shields.io/badge/Discord-fff?style=flat&logoColor=white&colorA=18181B&colorB=6134EB&logo=discord)](https://discord.com/users/896087803656560681)
[![C](https://img.shields.io/badge/Language-fff?style=flat&logoColor=white&colorA=18181B&colorB=6134EB&logo=c)](https://www.w3schools.com/c/c_intro.php)
[![CMake](https://img.shields.io/badge/Built%20with%20CMake-fff?style=flat&logoColor=white&colorA=18181B&colorB=6134EB&logo=cmake)](https://cmake.org)
[![License](https://img.shields.io/badge/License%20MIT-fff?style=flat&colorB=6134EB)](LICENSE)

## 🚀 What is FEST?

FEST is a high-performance C library that collects comprehensive Windows system information in JSON format. Built with speed and efficiency in mind, it's perfect for system monitoring, diagnostics, and QA testing.

**Born for the QA labs at Axioo Indonesia**, FEST delivers the system details you need without the overhead.

## ✨ Why FEST?

- **Fast & Lightweight**: Written in C for maximum performance
- **Minimal Footprint**: Designed to run without impacting system performance
- **Comprehensive Coverage**: Collects detailed hardware and system metrics
- **Responsive Monitoring**: Configurable refresh rates down to sub-100ms
- **Simple Integration**: Clean DLL interface with straightforward API
- **Smart Data Handling**: Automatically distinguishes between static and dynamic information

## 🛠️ Under the Hood

- **Optimized C Code** keeps things running smoothly
- **Multi-threading Support** with proper mutex implementation
- **Smart Data Refresh** only updates information that changes frequently
- **High-precision Timers** for accurate monitoring intervals

> 😄 Fun fact: Since I had no idea how to handle JSON in C (there's no built-in support), I just wrote my own JSON generator from scratch in [`json_structure.h`](https://github.com/ifeiera/fest/blob/main/include/json_structure.h) and [`json_structure.c`](https://github.com/ifeiera/fest/blob/main/src/json_structure.c) by manually formatting strings.

```c
// Example of how we manually format JSON from json_structure.c
static void appendGPUInfo(char **buffer, size_t *bufferSize, size_t *position, GPUList *gpuList)
{
    appendString(buffer, bufferSize, position, "  \"gpu\": [\n");
    for (UINT i = 0; i < gpuList->count; i++)
    {
        char temp[1024];
        GPUInfo *gpu = &gpuList->gpus[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"name\": \"%s\",\n"
                    "      \"vram\": %llu,\n"
                    "      \"shared_memory\": %llu,\n"
                    "      \"type\": \"%s\"\n"
                    "    }%s",
                    getGPUName(gpu),
                    getGPUDedicatedMemory(gpu),
                    getGPUSharedMemory(gpu),
                    isIntegratedGPU(gpu) ? "iGPU" : "dGPU",
                    i < gpuList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ],\n");
}
```

## 📊 System Information Coverage

### Hardware & System Metrics

| Component       | Information Collected                                                                                 |
| --------------- | ----------------------------------------------------------------------------------------------------- |
| **GPU**         | Model name, Dedicated VRAM, Shared memory, Type (integrated/discrete)                                 |
| **CPU**         | Name, Cores, Threads, Clock speed                                                                     |
| **Motherboard** | Manufacturer, Product name, Serial number, BIOS version/serial, System SKU                            |
| **RAM**         | Total/Available/Used memory, Usage percentage, Slot details (location, capacity, speed, manufacturer) |
| **Storage**     | Drive info (letter, type, model), Interface, Size metrics (total, free, used)                         |
| **Network**     | Device name, MAC address, IP address, Connection status (works with Ethernet, Wi-Fi, Bluetooth)       |
| **Battery**     | Charge percentage, Power status, Auto desktop/notebook detection                                      |
| **Monitor**     | Resolution, Primary status, Aspect ratio, Refresh rate, Size, Manufacturer, Device ID                 |

## 🧩 Smart Static/Dynamic Data Management

FEST intelligently separates static information (data that rarely changes) from dynamic information (data that updates frequently) to optimize performance:

```c
// Static data is collected only once at startup
g_MonitorContext.staticInfo.cpuList = getCPUList();
g_MonitorContext.staticInfo.gpuList = getGPUList();
g_MonitorContext.staticInfo.mbInfo = getMotherboardInfo();
g_MonitorContext.staticInfo.audioList = getAudioList();
g_MonitorContext.staticInfo.monitorList = getMonitorList();

// Dynamic data is refreshed on each monitoring cycle
g_MonitorContext.dynamicInfo.memInfo = getMemoryInfo();
g_MonitorContext.dynamicInfo.storageList = getStorageList();
g_MonitorContext.dynamicInfo.batteryInfo = getBatteryInfo();
g_MonitorContext.dynamicInfo.networkList = getNetworkList();
```

This design makes it extremely easy to:

1. Control which data is refreshed and when
2. Add new data points to either static or dynamic collections
3. Optimize performance by only updating what changes

## 🧠 Extensible WMI-Based Architecture

FEST's codebase is built on a clean abstraction of Windows Management Instrumentation through our [`wmi_helper.h`](https://github.com/ifeiera/fest/blob/main/include/wmi_helper.h) and [`wmi_helper.c`](https://github.com/ifeiera/fest/blob/main/src/wmi_helper.c) modules. This makes adding new system metrics straightforward:

```c
#include "wmi_helper.h"

// Example: Getting additional hardware info is as simple as:
void getMyCustomInfo() {
    WMISession *session = initializeWMI();
    IEnumWbemClassObject *pEnumerator = NULL;

    // Just write your WQL query
    if (executeWQLQuery(session, L"SELECT * FROM Win32_YourComponent", &pEnumerator)) {
        // Process your results
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK) {
            char buffer[256];
            if (getWMIPropertyString(pclsObj, L"PropertyName", buffer, sizeof(buffer))) {
                printf("Found property: %s\n", buffer);
            }
            pclsObj->lpVtbl->Release(pclsObj);
        }
    }

    cleanupWMI(session);
}
```

The WMI helper encapsulates all the COM initialization complexity and provides a clean interface for querying Windows Management Instrumentation - making FEST highly extensible for any system information you might need in the future.

## 🔌 Integration API

FEST exposes a simple C interface through its DLL:

```c
// Start collecting system info with specified refresh interval (in milliseconds)
BOOL startSystemMonitoring(int updateIntervalMs);

// Stop monitoring
void stopSystemMonitoring(void);

// Adjust refresh rate on the fly
void setUpdateInterval(int updateIntervalMs);

// Function type for receiving JSON data
typedef void (*SystemInfoCallback)(const char *jsonData);

// Register your callback to receive system information
void setSystemInfoCallback(SystemInfoCallback callback);
```

## 📝 Quick Start Example

```c
#include "system_info_dll.h"
#include <stdio.h>

void handleSystemInfo(const char* jsonData) {
    // Do something with your JSON data
    printf("System Info: %s\n", jsonData);
}

int main() {
    // Set your callback function
    setSystemInfoCallback(handleSystemInfo);

    // Start monitoring with 500ms refresh rate
    if (startSystemMonitoring(500)) {
        printf("Monitoring started successfully\n");

        // Your application code here...
        Sleep(10000);  // Just wait for 10 seconds

        // When you're done
        stopSystemMonitoring();
    }

    return 0;
}
```

FEST's flexible callback-based design enables a wide range of applications. Taking inspiration from other hardware monitoring solutions, FEST can power web-based hardware diagnostics through a local service that bridges the gap between web applications and low-level system information. This approach enables web developers to create hardware management portals, diagnostic tools, and system maintenance applications without the limitations of browser sandboxing.

The JSON output format makes it particularly easy to integrate with modern web applications, while the modular C architecture ensures minimal resource usage on the client system.

## 📖 Code Structure

The FEST codebase follows a modular design pattern:

- **Core Engine**: [`system_info_dll.c`](https://github.com/ifeiera/fest/blob/main/src/system_info_dll.c) for thread management and data collection orchestration
- **WMI Helpers**: [`wmi_helper.h`](https://github.com/ifeiera/fest/blob/main/include/wmi_helper.h) and [`wmi_helper.c`](https://github.com/ifeiera/fest/blob/main/src/wmi_helper.c) for clean WMI abstraction
- **JSON Formatting**: [`json_structure.h`](https://github.com/ifeiera/fest/blob/main/include/json_structure.h) and [`json_structure.c`](https://github.com/ifeiera/fest/blob/main/src/json_structure.c) for hand-crafted JSON output
- **Data Collection**: Specialized modules for each system component
- **Memory Management**: Careful allocation and cleanup to prevent leaks

The monitoring thread function maintains data coherence with proper mutex locking while efficiently managing refresh cycles based on the configured interval.

## 🏗️ Building FEST

Requirements:

- CMake (3.10+)
- C compiler (MSVC recommended for Windows)

```bash
# Clone the repository
git clone https://github.com/ifeiera/fest.git
cd fest

# Build the project
mkdir build
cd build
cmake ..
cmake --build .
```

[![CMake Build & Test](https://github.com/ifeiera/system-info-c/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/ifeiera/system-info-c/actions/workflows/cmake-single-platform.yml)

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to contribute to this project.
