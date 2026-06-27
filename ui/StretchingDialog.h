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
    void onAutoClicked();
    void onParameterChanged();
    void updatePreview();

private:
    WorkspaceImageWindow* getActiveImageWindow() const;
    void syncUiFromValues();

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
    QPushButton* m_autoBtn;
    QTimer* m_previewTimer;

    // Internal values
    bool m_isGhsMode = false;
    double m_blackpoint = 0.0;
    double m_whitepoint = 1.0;
    double m_midpoint = 0.5;

    double m_spPoint = 0.5;
    double m_stretchFactor = 3.0;
    double m_shadowProtect = 0.0;
    double m_highlightProtect = 1.0;

    bool m_colorPreserving = true;
    int m_threads = -1;
};

} // namespace blastro
