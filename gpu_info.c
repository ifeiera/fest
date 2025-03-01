#include "gpu_info.h"

DEFINE_GUID(IID_IDXGIFactory, 0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69);

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

    // Inisialisasi DXGI Factory
    hr = CreateDXGIFactory(&IID_IDXGIFactory, (void **)&factory);
    if (FAILED(hr))
    {
        free(list);
        return NULL;
    }

    // Hitung jumlah adapter terlebih dahulu
    while (factory->lpVtbl->EnumAdapters(factory, i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        adapter->lpVtbl->Release(adapter);
        i++;
    }

    list->count = i;
    list->gpus = (GPUInfo *)malloc(sizeof(GPUInfo) * list->count);

    // Dapatkan informasi setiap adapter
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
                wcstombs_s(&convertedChars, gpu->name, sizeof(gpu->name),
                           adapterDesc.Description, sizeof(gpu->name));

                gpu->dedicatedMemory = adapterDesc.DedicatedVideoMemory;
                gpu->sharedMemory = adapterDesc.SharedSystemMemory;
                gpu->isIntegrated = (adapterDesc.DedicatedVideoMemory < 512 * 1024 * 1024);
                gpu->adapterIndex = i;
            }
            adapter->lpVtbl->Release(adapter);
        }
    }

    factory->lpVtbl->Release(factory);
    return list;
}

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

const char *getGPUName(const GPUInfo *gpu)
{
    return gpu->name;
}

UINT64 getGPUDedicatedMemory(const GPUInfo *gpu)
{
    return gpu->dedicatedMemory;
}

UINT64 getGPUSharedMemory(const GPUInfo *gpu)
{
    return gpu->sharedMemory;
}

int isIntegratedGPU(const GPUInfo *gpu)
{
    return gpu->isIntegrated;
}
