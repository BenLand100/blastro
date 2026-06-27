add_library(blastro_algorithms
    Algorithm.cpp
    PixelMathAlgorithm.cpp
    StackingAlgorithm.cpp
    CalibrationAlgorithm.cpp
    Solver.cpp
    StarFinder.cpp
    ConstellationMatcher.cpp
    Warping.cpp
    BackgroundExtractor.cpp
    StretchingAlgorithm.cpp
    DebayerAlgorithm.cpp
    RegisterAlgorithm.cpp
    AlignAlgorithm.cpp
    BackgroundExtractionAlgorithm.cpp
    StretchingAlgorithmWrapper.cpp
)
target_include_directories(blastro_algorithms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(blastro_algorithms PUBLIC blastro_core blastro_io OpenMP::OpenMP_CXX)
