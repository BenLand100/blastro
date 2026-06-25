#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

namespace blastro {

class StackingDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StackingDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StackingDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stacking"; }

private slots:
    void onRejectionChanged(int index);
    void onRunClicked();

private:
    QComboBox* m_targetBatchCombo;
    QLineEdit* m_outputName;
    QComboBox* m_methodCombo;
    QComboBox* m_rejectionCombo;

    // Parameters Group
    QGroupBox* m_paramGroup;
    QLineEdit* m_sigmaLowEdit;
    QLineEdit* m_sigmaHighEdit;
    QLineEdit* m_quantileLowEdit;
    QLineEdit* m_quantileHighEdit;
    
    QLabel* m_sigmaLowLabel;
    QLabel* m_sigmaHighLabel;
    QLabel* m_quantileLowLabel;
    QLabel* m_quantileHighLabel;
};

} // namespace blastro
