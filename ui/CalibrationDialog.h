#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace blastro {

class CalibrationDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    CalibrationDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~CalibrationDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Calibration"; }
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QString m_outputPattern;
    QComboBox* m_biasCombo;
    QComboBox* m_darkCombo;
    QComboBox* m_flatCombo;
};

} // namespace blastro
