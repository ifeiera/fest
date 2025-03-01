cmake_minimum_required(VERSION 3.10)
project(system_info C)

# Set MSVC runtime library to static
if(MSVC)
    # Use static runtime
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Add library
add_library(system_info SHARED
    system_info_dll.c
    gpu_info.c
    wmi_helper.c
    motherboard_info.c
    cpu_info.c
    memory_info.c
    storage_info.c
    network_info.c
    audio_info.c
    battery_info.c
    monitor_info.c
    json_structure.c
)

# Define SYSTEM_INFO_EXPORTS for DLL build
target_compile_definitions(system_info PRIVATE SYSTEM_INFO_EXPORTS)

# Link required libraries
target_link_libraries(system_info dxgi d3d11 wbemuuid oleaut32 ole32 pdh iphlpapi setupapi)

# Add test application
add_executable(test_app test_app.c)
target_link_libraries(test_app system_info) 
