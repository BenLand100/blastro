/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "core/WorkspaceRegistry.h"
#include <QMdiArea>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

namespace blastro {

class MainWindow;
class StretchingDialog;
class BackgroundExtractionDialog;
class StackingDialog;
class RegisterDialog;
class StarFindingDialog;
class AlignDialog;
class DebayerDialog;
class CalibrationDialog;
class PixelMathDialog;
class PreprocessingWizardDialog;
class PlatesolveDialog;
class LogWindow;

/// Struct holding concrete dialog pointers for UI serialization.
struct UIDialogSet {
    StretchingDialog*           stretching  = nullptr;
    BackgroundExtractionDialog* bge         = nullptr;
    StackingDialog*             stacking    = nullptr;
    RegisterDialog*             registerDlg = nullptr;
    StarFindingDialog*          starFinding = nullptr;
    AlignDialog*                align       = nullptr;
    DebayerDialog*              debayer     = nullptr;
    CalibrationDialog*          calibration = nullptr;
    PixelMathDialog*            pixelMath   = nullptr;
    PreprocessingWizardDialog*  ppw         = nullptr;
    PlatesolveDialog*           platesolve  = nullptr;
    LogWindow*                  logWindow   = nullptr;
};

/// High-level UI and application layout serializer.
class ProjectUISerializer {
public:
    static bool saveProject(const QString& projectDir,
                            WorkspaceRegistry& workspace,
                            QMdiArea* mdi,
                            const UIDialogSet& dialogs,
                            MainWindow* mainWin);

    static bool loadProject(const QString& projectDir,
                            WorkspaceRegistry& workspace,
                            QMdiArea* mdi,
                            MainWindow* mainWin,
                            const UIDialogSet& dialogs);

    static bool saveSession(const QString& sessionPath,
                            QMdiArea* mdi,
                            const UIDialogSet& dialogs,
                            MainWindow* mainWin);

    static bool loadSession(const QString& sessionPath,
                            QMdiArea* mdi,
                            const UIDialogSet& dialogs,
                            MainWindow* mainWin);
};

} // namespace blastro
