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
    Logger.cpp
)
target_include_directories(blastro_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/thirdparty/PCL/include
)
target_link_libraries(blastro_core 
    PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets
    PRIVATE ${CMAKE_DL_LIBS}
)

