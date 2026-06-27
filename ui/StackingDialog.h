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
