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

class QMainWindow;

namespace blastro {

/// Aggregates serializable dialog instances as generic QObject pointers for ProjectSerializer.
struct DialogSet {
    QObject* stretching  = nullptr;
    QObject* bge        = nullptr;
    QObject* stacking    = nullptr;
    QObject* registerDlg = nullptr;
    QObject* starFinding = nullptr;
    QObject* align       = nullptr;
    QObject* debayer     = nullptr;
    QObject* calibration = nullptr;
    QObject* pixelMath   = nullptr;
    QObject* ppw         = nullptr;
    QObject* platesolve  = nullptr;
    QObject* logWindow   = nullptr;
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
