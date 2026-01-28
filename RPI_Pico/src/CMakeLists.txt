add_executable(${PROJECT_NAME}
    main.c
    # Adaugă aici și alte fișiere .c din src dacă ai
)

# Spunem unde sunt fișierele de FreeRTOS pe care le-ai pus în lib
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0
)

# Adăugăm sursele FreeRTOS direct în build-ul tău
target_sources(${PROJECT_NAME} PRIVATE
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/port.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
)

# Linkăm librăriile de bază din Pico SDK
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
)

# Activăm output-ul pe USB ca să vedem printf()
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Generăm fișierele .uf2
pico_add_extra_outputs(${PROJECT_NAME})