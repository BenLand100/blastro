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
#include <QCheckBox>

namespace blastro {

class PlatesolveDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    PlatesolveDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~PlatesolveDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Platesolve"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();

private:
    void populateHintsFromElement(const QString& name);

    QComboBox* m_targetInputCombo;
    QComboBox* m_solverCombo;
    QDoubleSpinBox* m_raHintSpin;
    QDoubleSpinBox* m_decHintSpin;
    QDoubleSpinBox* m_focalLengthSpin;
    QDoubleSpinBox* m_pixelSizeSpin;
    QCheckBox* m_blindSolveChk;
};

} // namespace blastro
