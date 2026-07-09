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
#include <QWidget>
#include <QJsonObject>
#include <map>
#include <string>

namespace blastro {

class AlgorithmDialog : public QWidget {
    Q_OBJECT
public:
    AlgorithmDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~AlgorithmDialog() override = default;

    virtual std::map<std::string, std::string> getConfig() const = 0;
    virtual std::string algorithmName() const = 0;
    virtual void refreshWorkspaceElements() {}

    // Project / session persistence
    virtual QJsonObject serializeState() const { return {}; }
    virtual void restoreState(const QJsonObject&) {}

public slots:
    void onClose();

signals:
    void algorithmExecuted(const std::string& name, const std::map<std::string, std::string>& config);

protected:
    WorkspaceRegistry& m_workspace;
};

} // namespace blastro
