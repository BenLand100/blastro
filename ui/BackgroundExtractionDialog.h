#pragma once
#include "AlgorithmDialog.h"
#include "WorkspaceImageWindow.h"
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

namespace blastro {

class BackgroundExtractionDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~BackgroundExtractionDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "BackgroundExtraction"; }

private slots:
    void onApplyClicked();
    void onPrefsClicked();

private:
    WorkspaceImageWindow* getActiveImageWindow() const;

    QSlider* m_orderSlider;
    QSpinBox* m_orderSpin;
    QSlider* m_sigmaSlider;
    QDoubleSpinBox* m_sigmaSpin;

    QCheckBox* m_equalizeChk;

    // Advanced Preferences
    double m_sampleFrac = 0.01;
    double m_huberDelta = 5.0;
    int m_threads = -1;
};

} // namespace blastro
