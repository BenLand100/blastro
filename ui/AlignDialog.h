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
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace blastro {

class AlignDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    AlignDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~AlignDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Align"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QString m_outputPattern;
    QComboBox* m_drizzleCombo;
    QComboBox* m_refModeCombo;
    QComboBox* m_interpolationCombo;

    // Advanced Preferences
    int m_threads = -1;
    bool m_evictCache = true;
};

} // namespace blastro
