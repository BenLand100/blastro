#include "AlgorithmDialog.h"

namespace blastro {

AlgorithmDialog::AlgorithmDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : QDialog(parent), m_workspace(workspace) {
    // Set default styling suitable for image processing
    setStyleSheet(
        "QDialog { background-color: #2b2b2b; color: #ffffff; }"
        "QLabel { color: #ffffff; }"
        "QPushButton { background-color: #007acc; color: #ffffff; border: none; padding: 6px 12px; border-radius: 3px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0098ff; }"
        "QPushButton:pressed { background-color: #005999; }"
        "QLineEdit { background-color: #3b3b3b; color: #ffffff; border: 1px solid #555; padding: 4px; border-radius: 2px; }"
        "QComboBox { background-color: #3b3b3b; color: #ffffff; border: 1px solid #555; padding: 4px; border-radius: 2px; }"
    );
}

} // namespace blastro
