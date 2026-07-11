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
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTimer>
#include <QTabWidget>

namespace blastro {

class HistogramWidget;

class StretchingDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StretchingDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StretchingDialog() override;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stretching"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onApplyClicked();
    void onPrefsClicked();
    void onTabChanged(int index);
    void onHtParamsChanged(double b, double w, double m);
    void onGhsParamsChanged(double sp, double d);
    void onGhsProtectionsChanged(double shadowProtect, double highlightProtect);
    void onCopyLiveStretch();
    void onParameterChanged();
    void updatePreview();

private:
    WorkspaceImageWindow* getActiveImageWindow() const;
    void syncUiFromValues();

    enum class ConstraintSource {
        SymmetryPoint,
        ShadowProtect,
        HighlightProtect
    };
    void enforceGhsConstraints(ConstraintSource source);

    // Dialog layout components
    QTabWidget* m_tabWidget;
    HistogramWidget* m_histogramWidget;

    // HT Tab parameters
    QSlider* m_bSlider;
    QDoubleSpinBox* m_bSpin;
    QSlider* m_wSlider;
    QDoubleSpinBox* m_wSpin;
    QSlider* m_mSlider;
    QDoubleSpinBox* m_mSpin;

    // GHS Tab parameters
    QSlider* m_spSlider;
    QDoubleSpinBox* m_spSpin;
    QSlider* m_dSlider;
    QDoubleSpinBox* m_dSpin;
    QSlider* m_shadowSlider;
    QDoubleSpinBox* m_shadowSpin;
    QSlider* m_highlightSlider;
    QDoubleSpinBox* m_highlightSpin;

    // General controls
    QCheckBox* m_previewChk;
    QPushButton* m_copyStretchesBtn;
    QTimer* m_previewTimer;

    // Internal values
    bool m_isGhsMode = false;
    double m_blackpoint = 0.0;
    double m_whitepoint = 1.0;
    double m_midpoint = 0.5;

    double m_spPoint = 0.5;
    double m_stretchFactor = 0.0;
    double m_shadowProtect = 0.0;
    double m_highlightProtect = 1.0;

    bool m_colorPreserving = true;
    int m_threads = -1;
};

} // namespace blastro
