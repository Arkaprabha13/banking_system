cmake_minimum_required(VERSION 3.15)
project(BankingSystem VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
)

# Remove main.cpp from sources for library
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Create library
add_library(banking_lib ${SOURCES})

# Create executable
add_executable(banking_system src/main.cpp)

# Link libraries
target_link_libraries(banking_system 
    banking_lib 
    Threads::Threads
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(banking_system ws2_32)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(banking_system PRIVATE /W4)
else()
    target_compile_options(banking_system PRIVATE -Wall -Wextra -pedantic)
endif()

# Create data directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/users)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/accounts)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/transactions)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/logs)

# Install targets
install(TARGETS banking_system DESTINATION bin)
install(DIRECTORY data/ DESTINATION data)

# Tests (optional)
# enable_testing()
# add_subdirectory(tests)
