cmake_minimum_required(VERSION 3.10)
project(chip8)

# Set a variable for our SDL2 directory
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Tell the compiler where to find headers
include_directories("${SDL2_DIR}/include")

# Tell the linker where to find library files
link_directories("${SDL2_DIR}/lib")

# Now, define the executable
add_executable(chip8 main.cpp chip8.cpp )

# Finally, tell the executable which libraries to link
target_link_libraries(chip8 mingw32 SDL2main SDL2)