cmake_minimum_required(VERSION 3.10)

project(Clox C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Get all .c files in src/
file(GLOB SOURCES "src/*.c")

# Add AddressSanitizer flags
add_compile_options(-g -fsanitize=address -fno-omit-frame-pointer)
add_link_options(-fsanitize=address)

# Add the executable
add_executable(clox ${SOURCES})

