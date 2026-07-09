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
#include <QDoubleSpinBox>
#include <QSpinBox>

namespace blastro {

class RegisterDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    RegisterDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~RegisterDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Register"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QComboBox* m_methodCombo;
    QDoubleSpinBox* m_snrSpin;
    QDoubleSpinBox* m_minFwhmSpin;
    QSpinBox* m_refIdxSpin;

    // Advanced Preferences
    int m_maxStars = 250;
    double m_simplexTol = 1e-4;
    int m_simplexMaxIter = 500;
    double m_matchTol = 1.5;
    double m_maxEccentricity = 0.85;
    int m_threads = -1;
};

} // namespace blastro
