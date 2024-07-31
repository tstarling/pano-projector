add_executable(pano-projector
        Command.cpp
        extractFace.cpp
        FaceCommand.cpp
        FaceDiagramCommand.cpp
        FaceInfo.cpp
        InputImage.cpp
        InputImageFactory.cpp
        InputJpeg.cpp
        IntegerCropRect.cpp
        main.cpp
        MemoryBudget.cpp
        OutputBase.cpp
        OutputImage.cpp
        OutputPyramid.cpp
        OutputTiler.cpp
        PyramidCommand.cpp
)

# Need std::filesystem and std::bit_width
target_compile_features(pano-projector PRIVATE cxx_std_20)

target_link_libraries(pano-projector Boost::program_options)
target_link_libraries(pano-projector ${JPEG_LIBRARIES})

install(TARGETS pano-projector)
