cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME lvgl_project)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# 自动把下面所有子目录递归出来 .c 源码文件
file(GLOB_RECURSE user
    BSP/*.c
    lvgl/*.c
    FreeRTOS/*.c
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${user}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/BSP/VL53L0X_LaserRanging/
    ${PROJECT_SOURCE_DIR}/BSP/VL53L0X_LaserRanging/Api/core/inc
    ${PROJECT_SOURCE_DIR}/BSP/VL53L0X_LaserRanging/Api/platform/inc
    
    ${PROJECT_SOURCE_DIR}/lvgl/
    ${PROJECT_SOURCE_DIR}/lvgl/src/
    
    ${PROJECT_SOURCE_DIR}/FreeRTOS/include/
    ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM4F
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# 开启 printf 的浮点数支持
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE 
    -u _printf_float
)
