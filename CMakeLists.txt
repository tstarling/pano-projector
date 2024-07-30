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

add_executable(pano-projector
		src/Command.cpp
		src/extractFace.cpp
		src/FaceCommand.cpp
		src/FaceDiagramCommand.cpp
		src/FaceInfo.cpp
		src/InputImage.cpp
		src/InputImageFactory.cpp
		src/InputJpeg.cpp
		src/IntegerCropRect.cpp
		src/main.cpp
		src/MemoryBudget.cpp
		src/OutputBase.cpp
		src/OutputImage.cpp
		src/OutputPyramid.cpp
		src/OutputTiler.cpp
		src/PyramidCommand.cpp
)

# Need std::filesystem and std::bit_width
target_compile_features(pano-projector PRIVATE cxx_std_20)

# Tested with 1.74 but probably works with any version
find_package(Boost 1.74 REQUIRED COMPONENTS program_options)
target_link_libraries(pano-projector Boost::program_options)

# Any libjpeg but please use libjpeg-turbo
find_package(JPEG REQUIRED)
target_link_libraries(pano-projector ${JPEG_LIBRARIES})

install(TARGETS pano-projector)
