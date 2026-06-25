cmake_minimum_required(VERSION 3.16)
project(Blastro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Try to find CCfits using find_library/find_path
find_library(CCFITS_LIBRARY CCfits REQUIRED)
find_path(CCFITS_INCLUDE_DIR CCfits/CCfits REQUIRED)

# Try to find cfitsio
find_library(CFITSIO_LIBRARY cfitsio REQUIRED)
find_path(CFITSIO_INCLUDE_DIR fitsio.h REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CCFITS_INCLUDE_DIR}
    ${CFITSIO_INCLUDE_DIR}
)

add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(algorithms)
add_subdirectory(ui)

add_executable(blastro main.cpp)
target_link_libraries(blastro PRIVATE
    blastro_core
    blastro_io
    blastro_algorithms
    blastro_ui
    Qt6::Core Qt6::Gui Qt6::Widgets
    ${CCFITS_LIBRARY}
    ${CFITSIO_LIBRARY}
)
