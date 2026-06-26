#pragma once
#include "AlgorithmDialog.h"
#include "WorkspaceImageWindow.h"
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTimer>

namespace blastro {

class BackgroundExtractionDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~BackgroundExtractionDialog() override;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "BackgroundExtraction"; }

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onApplyClicked();
    void onPrefsClicked();
    void onParameterChanged();
    void updatePreview();

private:
    WorkspaceImageWindow* getActiveImageWindow() const;
    ImageVariant cloneImage(const ImageVariant& img) const;

    QSlider* m_orderSlider;
    QSpinBox* m_orderSpin;
    QSlider* m_sigmaSlider;
    QDoubleSpinBox* m_sigmaSpin;

    QCheckBox* m_previewChk;
    QCheckBox* m_equalizeChk;

    QTimer* m_previewTimer;

    // Advanced Preferences
    double m_sampleFrac = 0.01;
    double m_huberDelta = 5.0;
    int m_threads = -1;
};

} // namespace blastro
