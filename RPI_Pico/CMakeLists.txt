cmake_minimum_required(VERSION 3.12)

# Setează calea SDK-ului (Dacă extensia nu o face automat, pune calea ta aici)
# set(PICO_SDK_PATH "/Users/mariusfiat/.pico-sdk/sdk/2.1.0")

# 1. Name project
SET(ProjectName RPI_Pico)

# 2. Configurare SDK
set(PICO_BOARD pico_w)
set(PICO_SDK_FETCH_FROM_GIT ON)
include(pico_sdk_import.cmake)

project(${ProjectName} C CXX ASM) # Adăugăm ASM pentru port-ul de FreeRTOS

# 3. Initialize Pico SDK
pico_sdk_init()

# 4. Configurare FreeRTOS (Calea manuală)
# În loc de include(...import.cmake), definim calea pentru subdirectorul src
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

# 5. Adăugăm codul sursă
add_subdirectory(src)