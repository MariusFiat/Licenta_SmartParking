cmake_minimum_required(VERSION 3.14)
project(rp2040_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Fetch gTest from GitHub
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#Test exec files
add_executable(run_tests 
    src/gpio/test_gpio.cpp
    src/adc/adc_test.cpp     
)

target_link_libraries(run_tests gtest_main)

target_compile_definitions(run_tests PRIVATE TEST)