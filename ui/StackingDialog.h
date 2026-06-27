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
#include <QLabel>

namespace blastro {

class StackingDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StackingDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StackingDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stacking"; }
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();
    void onRejectionChanged(const QString& text);

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QString m_outputPattern;
    QComboBox* m_methodCombo;
    QComboBox* m_rejectionCombo;
    QDoubleSpinBox* m_lowClipSpin;
    QDoubleSpinBox* m_highClipSpin;
    QLabel* m_clipLabel = nullptr;
    QLabel* m_slashLabel = nullptr;

    // Advanced Preferences
    std::string m_weightMethod = "none";
    int m_patchSize = 1024;
    int m_threads = -1;
};

} // namespace blastro
