add_library(blastro_core
    ImageBuffer.cpp
    GrayscaleImage.cpp
    RGBImage.cpp
    ImageBatch.cpp
    WorkspaceRegistry.cpp
    PCLBridge.cpp
    PCLStubs.cpp
    TempDirectory.cpp
    Preferences.cpp
)
target_include_directories(blastro_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/thirdparty/PCL/include
)
target_link_libraries(blastro_core 
    PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets
    PRIVATE ${CMAKE_DL_LIBS}
)

