#pragma once
#include "AlgorithmDialog.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>

namespace blastro {

class PixelMathDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    PixelMathDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~PixelMathDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "PixelMath"; }
    void refreshWorkspaceElements() override;

private slots:
    void onUseSingleExpressionChanged(bool checked);
    void onRunClicked();

private:
    QLineEdit* m_exprR;
    QLineEdit* m_exprG;
    QLineEdit* m_exprB;
    QLineEdit* m_exprK;

    QCheckBox* m_useSingleExpr;
    QRadioButton* m_rgbMode;
    QRadioButton* m_grayMode;

    QLineEdit* m_outputName;
    QComboBox* m_targetImageCombo;
    QRadioButton* m_createNewImage;
    QRadioButton* m_replaceTargetImage;
    QLabel* m_infoLabel = nullptr;
};

} // namespace blastro
