add_library(blastro_ui
    MainWindow.cpp
    WorkspaceArea.cpp
    ImageView.cpp
    BatchImageWidget.cpp
    HistogramWidget.cpp
    WorkspaceImageWindow.cpp
    AlgorithmDialog.cpp
    PixelMathDialog.cpp
    StackingDialog.cpp
    CalibrationDialog.cpp
)
target_include_directories(blastro_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(blastro_ui PUBLIC blastro_core blastro_algorithms blastro_io Qt6::Widgets Qt6::Gui Qt6::Core)
