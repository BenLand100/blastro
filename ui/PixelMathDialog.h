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
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>

namespace blastro {

class PixelMathDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    PixelMathDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~PixelMathDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "PixelMath"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;
    void refreshWorkspaceElements() override;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onUseSingleExpressionChanged(bool checked);
    void onRunClicked();
    void onVariableClicked(const QString& varName);

private:
    QLineEdit* m_exprR;
    QLineEdit* m_exprG;
    QLineEdit* m_exprB;
    QLineEdit* m_exprK;

    QCheckBox* m_useSingleExpr;
    QRadioButton* m_rgbMode;
    QRadioButton* m_grayMode;

    QLineEdit* m_outputName;
    QComboBox* m_targetImageCombo;
    QRadioButton* m_createNewImage;
    QRadioButton* m_replaceTargetImage;
    QLabel* m_infoLabel = nullptr;
    QLineEdit* m_lastFocusedExpr = nullptr;
};

} // namespace blastro
