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

namespace blastro {

class StackDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StackDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StackDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stacking"; } // Stacking is the name registered in MainWindow.cpp!

private slots:
    void onRunClicked();
    void onPrefsClicked();
    void onMethodChanged(int index);

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QComboBox* m_methodCombo;
    QComboBox* m_rejectionCombo;
    QDoubleSpinBox* m_lowClipSpin;
    QDoubleSpinBox* m_highClipSpin;

    // Advanced Preferences
    std::string m_weightMethod = "none";
    int m_stripHeight = 256;
    int m_threads = -1;
};

} // namespace blastro
