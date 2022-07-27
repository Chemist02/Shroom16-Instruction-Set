# specify minimum CMake version.
cmake_minimum_required(VERSION 3.10)

# set up project name and version.
project(Shroom16 VERSION 1.0)

# specify the minimum C++ standard.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add shroomasm executable, with all of its source files.
add_executable(shroomasm
	src/Main.cpp
	src/Instruction.cpp
	src/Parser.cpp
	src/InstructionWriter.cpp
)

# add zlib library to the project.
add_subdirectory(lib/zlib-1.2.12)

# add link directory so that the linker can find the zlib sources.
target_link_libraries(shroomasm zlibstatic)

# cmake chooses to rename this file for some reason, which results in the 
# program not being able to locate it. Thus, let's anme it back.
file(RENAME lib/zlib-1.2.12/zconf.h.included lib/zlib-1.2.12/zconf.h)

# add the zlib include directories so that schroomasm can find zlib.h.
target_include_directories(shroomasm PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/lib/zlib-1.2.12"
)
