# Set CMake minimum version
cmake_minimum_required(VERSION 3.15)

# Set project name
project(generic-timer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

# Set target
set(TARGET generic-timer)

# Gather source files
set(SOURCES main.cpp)

# Add subdirectory for timer_service library
add_subdirectory(timer_service)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE timer_service)

