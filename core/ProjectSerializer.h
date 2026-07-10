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
#include "WorkspaceRegistry.h"
#include <QMdiArea>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

// Forward declarations to avoid pulling all dialog headers into every TU
namespace blastro {
class StretchingDialog;
class BackgroundExtractionDialog;
class StackingDialog;
class RegisterDialog;
class AlignDialog;
class DebayerDialog;
class CalibrationDialog;
class PixelMathDialog;
class PreprocessingWizardDialog;
class PlatesolveDialog;
}
class QMainWindow;

namespace blastro {

/// Aggregates all serializable dialog instances for pass-through to ProjectSerializer.
struct DialogSet {
    StretchingDialog*          stretching  = nullptr;
    BackgroundExtractionDialog* bge        = nullptr;
    StackingDialog*            stacking    = nullptr;
    RegisterDialog*            registerDlg = nullptr;
    AlignDialog*               align       = nullptr;
    DebayerDialog*             debayer     = nullptr;
    CalibrationDialog*         calibration = nullptr;
    PixelMathDialog*           pixelMath   = nullptr;
    PreprocessingWizardDialog* ppw         = nullptr;
    PlatesolveDialog*          platesolve  = nullptr;
};

/// Central service for reading and writing project files and session files.
class ProjectSerializer {
public:
    static bool saveProject(const QString& projectDir,
                            WorkspaceRegistry& workspace,
                            QMdiArea* mdi,
                            const DialogSet& dialogs,
                            QMainWindow* mainWin);

    static bool loadProject(const QString& projectDir,
                            WorkspaceRegistry& workspace,
                            QMdiArea* mdi,
                            QMainWindow* mainWin,
                            const DialogSet& dialogs);

    static bool saveSession(const QString& sessionPath,
                            QMdiArea* mdi,
                            const DialogSet& dialogs,
                            QMainWindow* mainWin);

    static bool loadSession(const QString& sessionPath,
                            QMdiArea* mdi,
                            const DialogSet& dialogs,
                            QMainWindow* mainWin = nullptr);

    static QStringList externalReferences(const QString& projectDir,
                                          const WorkspaceRegistry& workspace);

    static bool copyReferencesIntoProject(const QString& projectDir,
                                          WorkspaceRegistry& workspace);

private:
    static QJsonObject serializeDialogs(const DialogSet& dialogs);
    static void restoreDialogs(const QJsonObject& obj, const DialogSet& dialogs);

    static QJsonObject serializeWorkspace(const QString& projectDir,
                                          const WorkspaceRegistry& workspace);
    static void restoreWorkspace(const QJsonObject& obj,
                                 WorkspaceRegistry& workspace,
                                 QMdiArea* mdi,
                                 QMainWindow* mainWin);

    static QJsonArray serializeMdiToolWindows(QMdiArea* mdi, const DialogSet& dialogs);
    static void restoreMdiToolWindows(const QJsonArray& arr,
                                      QMdiArea* mdi,
                                      const DialogSet& dialogs);
};

} // namespace blastro
