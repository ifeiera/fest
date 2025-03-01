# Add test executables
add_executable(test_storage tests_storage.c)
add_executable(test_memory tests_memory.c)
add_executable(test_cpu tests_cpu.c)
add_executable(test_gpu tests_gpu.c)
add_executable(test_motherboard tests_motherboard.c)
add_executable(test_network tests_network.c)
add_executable(test_audio tests_audio.c)
add_executable(test_monitor tests_monitor.c)
add_executable(test_battery tests_battery.c)
add_executable(test_summary tests_summary.c)

# Link with main library
target_link_libraries(test_storage systeminfo)
target_link_libraries(test_memory systeminfo)
target_link_libraries(test_cpu systeminfo)
target_link_libraries(test_gpu systeminfo)
target_link_libraries(test_motherboard systeminfo)
target_link_libraries(test_network systeminfo)
target_link_libraries(test_audio systeminfo)
target_link_libraries(test_monitor systeminfo)
target_link_libraries(test_battery systeminfo)
target_link_libraries(test_summary systeminfo)

# Add tests with working directory
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
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
    add_test(NAME TestSummary 
        COMMAND test_summary
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/$<CONFIG>)
endif() 
