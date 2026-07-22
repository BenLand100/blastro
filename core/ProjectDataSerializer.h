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
#include <QJsonObject>
#include <QStringList>

namespace blastro {

/// Headless serializer for WorkspaceRegistry elements and project directory assets.
/// Has zero dependencies on Qt GUI (QMdiArea, QWidget, QMainWindow).
class ProjectDataSerializer {
public:
    /// Serializes WorkspaceRegistry elements (GrayscaleImage, RGBImage, ImageBatch) to a JSON object.
    static QJsonObject serializeWorkspace(const QString& projectDir,
                                           const WorkspaceRegistry& workspace);

    /// Restores WorkspaceRegistry elements from a JSON object (for headless execution).
    static void restoreWorkspaceData(const QJsonObject& obj,
                                     const QString& projectDir,
                                     WorkspaceRegistry& workspace);

    /// Saves project data to projectDir/project.json (headless data only).
    static bool saveProjectData(const QString& projectDir,
                                const WorkspaceRegistry& workspace);

    /// Loads project data from projectDir/project.json into WorkspaceRegistry (headless).
    static bool loadProjectData(const QString& projectDir,
                                WorkspaceRegistry& workspace);

    /// Returns list of external image file paths referenced by batches in workspace.
    static QStringList externalReferences(const QString& projectDir,
                                           const WorkspaceRegistry& workspace);

    /// Copy external file references into project folder.
    static bool copyReferencesIntoProject(const QString& projectDir,
                                          WorkspaceRegistry& workspace);
};

} // namespace blastro
