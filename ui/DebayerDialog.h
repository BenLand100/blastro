#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace blastro {

class DebayerDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    DebayerDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~DebayerDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Debayer"; }
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QComboBox* m_patternCombo;
    QComboBox* m_methodCombo;

    // Advanced Preferences
    bool m_greenEqualize = true;
    int m_threads = -1;
};

} // namespace blastro
