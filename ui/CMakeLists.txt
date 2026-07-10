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
    DebayerDialog.cpp
    RegisterDialog.cpp
    AlignDialog.cpp
    BackgroundExtractionDialog.cpp
    PlatesolveDialog.cpp
    StretchingDialog.cpp
    LogWindow.cpp
    PreferencesWindow.cpp
    UpdateManagerDialog.cpp
    StatsPlotWidget.cpp
    BatchFilterDialog.cpp
    PreprocessingWizardDialog.cpp
)
target_include_directories(blastro_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(blastro_ui PUBLIC blastro_core blastro_algorithms blastro_io Qt6::Widgets Qt6::Gui Qt6::Core)
