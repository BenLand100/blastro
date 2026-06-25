add_library(blastro_io
    ImageReader.cpp
    ImageWriter.cpp
    QtImageIO.cpp
    FitsIO.cpp
)
target_include_directories(blastro_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(blastro_io PUBLIC blastro_core Qt6::Gui)
