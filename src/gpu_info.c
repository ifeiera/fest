#include "gpu_info.h"

// DXGI Factory interface GUID
DEFINE_GUID(IID_IDXGIFactory, 0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69);

/**
 * @brief Converts bytes to gigabytes
 *
 * @param bytes Memory size in bytes
 * @return double Memory size in gigabytes
 */
static double bytesToGB(UINT64 bytes)
{
    return (double)bytes / (1024 * 1024 * 1024);
}

/**
 * @brief Retrieves information about all graphics adapters in the system
 *
 * This function uses DirectX Graphics Infrastructure (DXGI) to enumerate
 * and collect information about graphics adapters including:
 * - Adapter name and description
 * - Dedicated video memory
 * - Shared system memory
 * - Integrated/Discrete GPU detection
 *
 * @return GPUList* Pointer to allocated list of GPU information, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeGPUList()
 */
GPUList *getGPUList(void)
{
    IDXGIFactory *factory = NULL;
    IDXGIAdapter *adapter = NULL;
    DXGI_ADAPTER_DESC adapterDesc;
    HRESULT hr;
    UINT i = 0;

    GPUList *list = (GPUList *)malloc(sizeof(GPUList));
    list->count = 0;
    list->gpus = NULL;

    // Create DXGI Factory to enumerate adapters
    hr = CreateDXGIFactory(&IID_IDXGIFactory, (void **)&factory);
    if (FAILED(hr))
    {
        free(list);
        return NULL;
    }

    // First pass: Count total number of graphics adapters
    while (factory->lpVtbl->EnumAdapters(factory, i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        adapter->lpVtbl->Release(adapter);
        i++;
    }

    list->count = i;
    list->gpus = (GPUInfo *)malloc(sizeof(GPUInfo) * list->count);

    // Second pass: Collect detailed adapter information
    for (UINT i = 0; i < list->count; i++)
    {
        hr = factory->lpVtbl->EnumAdapters(factory, i, &adapter);
        if (SUCCEEDED(hr))
        {
            hr = adapter->lpVtbl->GetDesc(adapter, &adapterDesc);
            if (SUCCEEDED(hr))
            {
                GPUInfo *gpu = &list->gpus[i];
                size_t convertedChars = 0;

                // Convert wide string description to narrow string
                wcstombs_s(&convertedChars, gpu->name, sizeof(gpu->name),
                           adapterDesc.Description, sizeof(gpu->name));

                // Store adapter memory information in GB
                gpu->dedicatedMemory = bytesToGB(adapterDesc.DedicatedVideoMemory);
                gpu->sharedMemory = bytesToGB(adapterDesc.SharedSystemMemory);

                // Detect integrated GPU based on dedicated memory size
                // Less than 512MB typically indicates integrated graphics
                gpu->isIntegrated = (adapterDesc.DedicatedVideoMemory < 512 * 1024 * 1024);
                gpu->adapterIndex = i;
            }
            adapter->lpVtbl->Release(adapter);
        }
    }

    factory->lpVtbl->Release(factory);
    return list;
}

/**
 * @brief Frees memory allocated for GPUList structure
 *
 * @param list Pointer to GPUList structure to be freed
 */
void freeGPUList(GPUList *list)
{
    if (list)
    {
        if (list->gpus)
        {
            free(list->gpus);
        }
        free(list);
    }
}

/**
 * @brief Gets the name/description of the GPU
 *
 * @param gpu Pointer to GPUInfo structure
 * @return const char* GPU name/description
 */
const char *getGPUName(const GPUInfo *gpu)
{
    return gpu->name;
}

/**
 * @brief Gets the amount of dedicated video memory
 *
 * @param gpu Pointer to GPUInfo structure
 * @return double Dedicated video memory in GB
 */
double getGPUDedicatedMemory(const GPUInfo *gpu)
{
    return gpu ? gpu->dedicatedMemory : 0.0;
}

/**
 * @brief Gets the amount of shared system memory
 *
 * @param gpu Pointer to GPUInfo structure
 * @return double Shared system memory in GB
 */
double getGPUSharedMemory(const GPUInfo *gpu)
{
    return gpu ? gpu->sharedMemory : 0.0;
}

/**
 * @brief Determines if the GPU is integrated or discrete
 *
 * @param gpu Pointer to GPUInfo structure
 * @return int 1 if integrated, 0 if discrete
 * @note Detection is based on dedicated memory size threshold
 */
int isIntegratedGPU(const GPUInfo *gpu)
{
    return gpu->isIntegrated;
}
