# specify minimum CMake version.
cmake_minimum_required(VERSION 3.10)

# set up project name and version.
project(Shroom16 VERSION 1.0)

# specify the minimum C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add shroomasm executable, with all of its source files.
add_executable(shroomasm
	src/Shroomasm.cpp
	src/Instruction.cpp
	src/Parser.cpp
	src/InstructionWriter.cpp
)

# add shroomemu executable, with all of its source files.
add_executable(shroomemu
	src/Shroomemu.cpp
	src/Instruction.cpp	
	src/Page437OutputScreen.cpp
	src/RegisterFile.cpp
	src/DataMemory.cpp
	src/PixelScreen.cpp
	src/Processor.cpp
)

# add zlib library to the project.
add_subdirectory(lib/zlib-1.3.2)

# add SFML library to project with static linking.
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(lib/SFML-3.1.0)

# add link directory so that the linker can find the zlib sources.
target_link_libraries(shroomasm zlibstatic)

# add link directory so that linker can find sfml sources.
target_link_libraries(shroomemu sfml-graphics)

# cmake chooses to rename this file for some reason, which results in the 
# program not being able to locate it. Thus, let's anme it back.
# file(RENAME lib/zlib-1.3.2/zconf.h.included lib/zlib-1.3.2/zconf.h)

# copy code page 437 character set png to build directory.
file(COPY ./assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# add the zlib include directories so that schroomasm can find zlib.h.
target_include_directories(shroomasm PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/lib/zlib-1.3.2"
)

target_include_directories(shroomemu PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/lib/SFML-3.1.0"
)
