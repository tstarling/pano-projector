project(pano-projector)

# Tested on 3.22 but probably works on 3.11
cmake_minimum_required(VERSION 3.22)

if (CMAKE_BUILD_TYPE STREQUAL "")
	message(FATAL_ERROR "Please set a build type e.g. cmake -DCMAKE_BUILD_TYPE=Release")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG -ggdb3")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")

add_compile_options(-Wall)

if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
	# Go faster
	add_compile_options(-march=skylake)
endif()

# Tested with 1.74 but probably works with any version
find_package(Boost 1.74 REQUIRED COMPONENTS program_options)

# Any libjpeg but please use libjpeg-turbo
find_package(JPEG REQUIRED)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)