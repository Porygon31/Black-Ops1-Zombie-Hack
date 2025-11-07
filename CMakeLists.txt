cmake_minimum_required(VERSION 3.15)

project(BO1_ESP
    VERSION 2.0.0
    DESCRIPTION "Modern C++ ESP for Call of Duty: Black Ops 1 Zombies"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BO1_ESP_BUILD_EXAMPLES "Build example programs" ON)
option(BO1_ESP_BUILD_TESTS "Build tests" OFF)

# Library sources (header-only library)
add_library(bo1_esp INTERFACE)

# Include directories
target_include_directories(bo1_esp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler options
if(MSVC)
    target_compile_options(bo1_esp INTERFACE
        /W4                 # Warning level 4
        /permissive-        # Standards conformance
        /Zc:__cplusplus     # Enable correct __cplusplus macro
    )
else()
    target_compile_options(bo1_esp INTERFACE
        -Wall
        -Wextra
        -pedantic
    )
endif()

# Main DLL target
add_library(bo1_esp_dll SHARED
    src/main.cpp
)

target_link_libraries(bo1_esp_dll PRIVATE bo1_esp)

# Output name
set_target_properties(bo1_esp_dll PROPERTIES
    OUTPUT_NAME "BO1_ESP"
    PREFIX ""
    SUFFIX ".dll"
)

# Installation
include(GNUInstallDirs)

install(TARGETS bo1_esp
    EXPORT bo1_esp-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/bo1_esp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT bo1_esp-targets
    FILE bo1_esp-config.cmake
    NAMESPACE bo1::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bo1_esp
)

# Example usage
if(BO1_ESP_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()

# Tests
if(BO1_ESP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
