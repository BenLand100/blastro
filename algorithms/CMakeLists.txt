add_library(blastro_algorithms
    Algorithm.cpp
    PixelMathAlgorithm.cpp
    StackingAlgorithm.cpp
    CalibrationAlgorithm.cpp
)
target_include_directories(blastro_algorithms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(blastro_algorithms PUBLIC blastro_core)
