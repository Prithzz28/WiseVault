cmake_minimum_required(VERSION 3.14)
project(WiseVault VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    src/main.cpp
)

# Header-only project: include directory
include_directories(include)

# Executable
add_executable(WiseVault ${SOURCES})

# Platform-specific linker flags
if(UNIX AND NOT APPLE)
    target_link_libraries(WiseVault PRIVATE m)
endif()

# Ensure data directory exists at build time
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Install target
install(TARGETS WiseVault DESTINATION bin)
