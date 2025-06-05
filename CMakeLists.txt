# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Declare the project name
project(MonteCarloPi)

# Tell CMake to use C++14
set(CMAKE_CXX_STANDARD 14)

# List all source files needed to build the executable
set(SOURCE_FILES
    src/main.cpp
    src/utils.cpp
    src/random_engine.cpp
    src/stratified_engine.cpp
    src/exponential_engine.cpp
)

# Add the src directory to the include path so headers can be found
include_directories(src)

# Define the executable target and link all source files into it
add_executable(monte_carlo_pi ${SOURCE_FILES})
