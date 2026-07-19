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
#include <QMdiSubWindow>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPointer>
#include <QPushButton>
#include <QButtonGroup>
#include <QTimer>
#include <QTabWidget>

class QStackedWidget;

namespace blastro {

class HtWidget;
class GhsWidget;
class CurvesWidget;

class StretchingDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StretchingDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StretchingDialog() override;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stretching"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;

    // Global preview management
    bool hasActivePreview() const override;
    void clearPreview() override;
    QMdiSubWindow* getTargetWindow() const override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onApplyClicked();
    void onPrefsClicked();
    void onTabChanged(int index);
    void onHtParamsChanged(const std::array<double, 6>& b, const std::array<double, 6>& w, const std::array<double, 6>& m);
    void onGhsParamsChanged(const std::array<double, 6>& sp, const std::array<double, 6>& d);
    void onGhsProtectionsChanged(const std::array<double, 6>& shadowProtect, const std::array<double, 6>& highlightProtect);
    void onCurveChanged(int channel, const std::vector<QPointF>& points);
    void onCopyLiveStretch();
    void onParameterChanged();
    void onChannelChanged(int id);
    void onResetStretchClicked();
    void updatePreview();
    ImageVariant applyCurrentStretch(const ImageVariant& baseImg, bool isPreview = false);
    void refreshHistogramAndCache();
    void onTargetImageChanged(QMdiSubWindow* sub);
    void onTargetImageUpdated();

private:
    void syncUiFromValues();

    enum class ConstraintSource {
        SymmetryPoint,
        ShadowProtect,
        HighlightProtect
    };
    void enforceGhsConstraints(ConstraintSource source);

    // Dialog layout components
    QTabWidget* m_tabWidget;
    QStackedWidget* m_histogramContainer; // To swap between HtWidget, GhsWidget, and CurvesWidget
    HtWidget* m_htWidget;
    GhsWidget* m_ghsWidget;
    CurvesWidget* m_curvesWidget;

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
    QButtonGroup* m_channelGroup;
    QCheckBox* m_previewChk;
    QPushButton* m_copyStretchesBtn;
    QTimer* m_previewTimer;

    // Internal values
    enum class StretchMode { HT, GHS, Curves };
    StretchMode m_mode = StretchMode::HT;
    
    enum class ActiveChannel {
        K = 0,
        R = 1,
        G = 2,
        B = 3,
        L = 4,
        S = 5
    };
    ActiveChannel m_activeChannel = ActiveChannel::K;
    bool m_channelsLinked = true;
    
    QPointer<QMdiSubWindow> m_currentTrackedSub;

    // HT Parameters (RGB, L, S)
    std::array<double, 3> m_blackpoint = {0.0, 0.0, 0.0};
    std::array<double, 3> m_whitepoint = {1.0, 1.0, 1.0};
    std::array<double, 3> m_midpoint = {0.5, 0.5, 0.5};
    double m_lBlackpoint = 0.0;
    double m_lWhitepoint = 1.0;
    double m_lMidpoint = 0.5;
    double m_sBlackpoint = 0.0;
    double m_sWhitepoint = 1.0;
    double m_sMidpoint = 0.5;

    // GHS Parameters (RGB, L, S)
    std::array<double, 3> m_spPoint = {0.5, 0.5, 0.5};
    std::array<double, 3> m_stretchFactor = {0.0, 0.0, 0.0};
    std::array<double, 3> m_shadowProtect = {0.0, 0.0, 0.0};
    std::array<double, 3> m_highlightProtect = {1.0, 1.0, 1.0};
    double m_lSpPoint = 0.5, m_lStretchFactor = 0.0, m_lShadowProtect = 0.0, m_lHighlightProtect = 1.0;
    double m_sSpPoint = 0.5, m_sStretchFactor = 0.0, m_sShadowProtect = 0.0, m_sHighlightProtect = 1.0;

    // Curves Parameters (K, R, G, B, L, S)
    std::array<std::vector<QPointF>, 6> m_curvePoints;

    int m_threads = -1;

    // Caching
    std::shared_ptr<ImageBuffer> m_cachedHBuf;
    std::shared_ptr<ImageBuffer> m_cachedSBuf;
    std::shared_ptr<ImageBuffer> m_cachedLBuf;
    void* m_cachedBaseImgPtr = nullptr;
    std::vector<std::vector<int>> m_cachedHistogram;
    int m_cachedHistogramChannel = -1;
};

} // namespace blastro
