#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>

namespace blastro {

class StackDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StackDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StackDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Stacking"; } // Stacking is the name registered in MainWindow.cpp!

private slots:
    void onRunClicked();
    void onPrefsClicked();
    void onMethodChanged(int index);

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QComboBox* m_methodCombo;
    QComboBox* m_rejectionCombo;
    QDoubleSpinBox* m_lowClipSpin;
    QDoubleSpinBox* m_highClipSpin;

    // Advanced Preferences
    std::string m_weightMethod = "none";
    int m_stripHeight = 256;
    int m_threads = -1;
};

} // namespace blastro
