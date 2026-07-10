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
#include "WorkspaceImageWindow.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>

namespace blastro {

class BackgroundExtractionDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~BackgroundExtractionDialog() override;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "BackgroundExtraction"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void onApplyClicked();
    void onPrefsClicked();
    void onGenerateGridClicked();
    void onClearPointsClicked();
    void onSubWindowActivated(QMdiSubWindow* subWindow);

private:
    WorkspaceImageWindow* getActiveImageWindow() const;
    QMdiArea* findMdiArea() const;
    void updateBgeModes();
    void disableAllBgeModes();

    QComboBox* m_methodCombo;
    QSlider* m_orderSlider;
    QSpinBox* m_orderSpin;
    QSlider* m_rbfSmoothingSlider;
    QDoubleSpinBox* m_rbfSmoothingSpin;

    QSlider* m_sigmaSlider;
    QDoubleSpinBox* m_sigmaSpin;

    QCheckBox* m_equalizeChk;

    // Grid Generation controls
    QSpinBox* m_gridColsSpin;
    QSpinBox* m_gridRowsSpin;
    QCheckBox* m_autoExcludeChk;
    QDoubleSpinBox* m_excludeThresholdSpin;

    // Advanced Preferences
    double m_sampleFrac = 0.01;
    double m_huberDelta = 5.0;
    int m_threads = -1;
};

} // namespace blastro
