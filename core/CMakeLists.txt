add_library(blastro_core
    ImageBuffer.cpp
    GrayscaleImage.cpp
    RGBImage.cpp
    ImageBatch.cpp
    WorkspaceRegistry.cpp
)
target_include_directories(blastro_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
