cmake_minimum_required(VERSION 3.11)
project(NESTRIS)

set(CMAKE_CXX_STANDARD 17)

# Include the directories with header files
include_directories(include)

# List your source files here.
file(GLOB SOURCES "src/*.cpp")

# Add an executable and link it with the source files
add_executable(NESTRIS nestris.cpp ${SOURCES})

# Add SFML as a subdirectory so it gets built along with your project
# Update this to the path where your SFML folder is
add_subdirectory(SFML-2.5.x)

# Link the SFML libraries to your executable
target_link_libraries(NESTRIS sfml-graphics sfml-window sfml-system sfml-audio)

