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

class GhsDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    GhsDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~GhsDialog() override;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Ghs"; }

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onApplyClicked();
    void onPrefsClicked();
    void onParameterChanged();
    void updatePreview();

private:
    WorkspaceImageWindow* getActiveImageWindow() const;

    QSlider* m_dSlider;
    QDoubleSpinBox* m_dSpin;
    QSlider* m_spSlider;
    QDoubleSpinBox* m_spSpin;
    QSlider* m_bSlider;
    QDoubleSpinBox* m_bSpin;

    QCheckBox* m_previewChk;

    QTimer* m_previewTimer;

    // Advanced Preferences
    double m_shadowProtect = 0.0;
    double m_highlightProtect = 0.0;
    bool m_colorPreserving = true;
    int m_threads = -1;
};

} // namespace blastro
