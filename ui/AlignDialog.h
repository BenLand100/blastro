#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace blastro {

class AlignDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    AlignDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~AlignDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Align"; }
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QLineEdit* m_outputName;
    QString m_outputPattern;
    QComboBox* m_drizzleCombo;

    // Advanced Preferences
    int m_stripHeight = 256;
    int m_threads = -1;
    bool m_evictCache = true;
};

} // namespace blastro
