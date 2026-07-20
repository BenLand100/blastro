# BLastro - Astronomical Image Processing Software
# Copyright (C) 2026 Benjamin Land
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.16)
project(BLastro LANGUAGES CXX)

# Configure RPATH to look in relative lib/ directory at runtime
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(UNIX AND NOT APPLE)
    # $ORIGIN/lib works if run from the parent directory of lib, and $ORIGIN/../lib works if run from bin/
    # We escape the $ to prevent CMake from evaluating it as a CMake variable.
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/lib:\$ORIGIN/../lib")
    set(CMAKE_BUILD_RPATH "\$ORIGIN/lib:\$ORIGIN/../lib")
elseif(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/lib:@executable_path/../lib")
    set(CMAKE_BUILD_RPATH "@executable_path/lib:@executable_path/../lib")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(__PCL_LINUX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenMP REQUIRED)

# Try to find CCfits using find_library/find_path
find_library(CCFITS_LIBRARY CCfits REQUIRED)
find_path(CCFITS_INCLUDE_DIR CCfits/CCfits REQUIRED)

# Try to find cfitsio
find_library(CFITSIO_LIBRARY cfitsio REQUIRED)
find_path(CFITSIO_INCLUDE_DIR fitsio.h REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CMAKE_SOURCE_DIR}/thirdparty/PCL/include
    ${CCFITS_INCLUDE_DIR}
    ${CFITSIO_INCLUDE_DIR}
)

add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(algorithms)
add_subdirectory(ui)

add_executable(blastro main.cpp ui/resources.qrc)
target_link_libraries(blastro PRIVATE
    blastro_core
    blastro_io
    blastro_algorithms
    blastro_ui
    Qt6::Core Qt6::Gui Qt6::Widgets
    ${CCFITS_LIBRARY}
    ${CFITSIO_LIBRARY}
    OpenMP::OpenMP_CXX
)

add_executable(test_algorithms tests/test_algorithms.cpp)
target_link_libraries(test_algorithms PRIVATE
    blastro_core
    blastro_ui
    blastro_algorithms
    Qt6::Core Qt6::Gui Qt6::Widgets
    OpenMP::OpenMP_CXX
)
