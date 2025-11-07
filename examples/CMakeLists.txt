# Examples for BO1 ESP library

# Basic usage example
add_executable(example_basic basic_usage.cpp)
target_link_libraries(example_basic PRIVATE bo1_esp)

# Custom configuration example
add_executable(example_custom_config custom_config.cpp)
target_link_libraries(example_custom_config PRIVATE bo1_esp)
