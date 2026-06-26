#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>

namespace blastro {

class RegisterDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    RegisterDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~RegisterDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "Register"; }
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QComboBox* m_methodCombo;
    QDoubleSpinBox* m_snrSpin;
    QDoubleSpinBox* m_minFwhmSpin;
    QSpinBox* m_refIdxSpin;

    // Advanced Preferences
    int m_maxStars = 250;
    double m_simplexTol = 1e-4;
    int m_simplexMaxIter = 500;
    double m_matchTol = 1.5;
    double m_maxEccentricity = 0.85;
    int m_threads = -1;
};

} // namespace blastro
