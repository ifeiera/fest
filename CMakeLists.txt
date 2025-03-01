cmake_minimum_required(VERSION 3.10)
project(systeminfo C)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Build shared library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(BUILD_SHARED_LIBS ON)

# Set MSVC runtime library to static
if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Source files
# For dll build
set(SOURCES
    src/system_info_dll.c
    src/gpu_info.c
    src/wmi_helper.c
    src/motherboard_info.c
    src/cpu_info.c
    src/memory_info.c
    src/storage_info.c
    src/network_info.c
    src/audio_info.c
    src/battery_info.c
    src/monitor_info.c
    src/json_structure.c
    src/system_info.rc
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Create shared library
add_library(systeminfo SHARED ${SOURCES})

# Define SYSTEM_INFO_EXPORTS for DLL build
target_compile_definitions(systeminfo PRIVATE SYSTEM_INFO_EXPORTS)

# Set DLL properties
set_target_properties(systeminfo PROPERTIES
    VERSION 0.4.2
    SOVERSION 1
)

# Link required libraries
target_link_libraries(systeminfo dxgi d3d11 wbemuuid oleaut32 ole32 pdh iphlpapi setupapi)

# Add test application
add_executable(test_app src/test_app.c)
target_link_libraries(test_app systeminfo)

# Enable testing with configuration
enable_testing()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Copy DLL to test directory post-build
    add_custom_command(TARGET systeminfo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:systeminfo>
        ${CMAKE_BINARY_DIR}/tests/$<CONFIG>/)

    # Add tests with DLL path
    add_test(NAME TestStorage 
        COMMAND test_storage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestMemory 
        COMMAND test_memory
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestCPU 
        COMMAND test_cpu
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestGPU 
        COMMAND test_gpu
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestMotherboard 
        COMMAND test_motherboard
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestNetwork 
        COMMAND test_network
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestAudio 
        COMMAND test_audio
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestMonitor 
        COMMAND test_monitor
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
    add_test(NAME TestBattery 
        COMMAND test_battery
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
endif()

# Add test directory
add_subdirectory(tests) 
