#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>

namespace blastro {

class PreferencesWindow : public QWidget {
    Q_OBJECT
public:
    explicit PreferencesWindow(QWidget* parent = nullptr);
    ~PreferencesWindow() override = default;

private slots:
    void onSaveClicked();
    void onBrowseModule();
    void onBrowseLib();
    void onBrowseLibrary();
    void onBrowseTemp();
    void onBrowseIntermediate();
    void onAddRepo();
    void onRemoveRepo();

private:
    QLineEdit* m_moduleEdit;
    QLineEdit* m_libEdit;
    QLineEdit* m_libraryEdit;
    QCheckBox* m_tensorflowChk;
    QLineEdit* m_tempEdit;
    QLineEdit* m_intermediateEdit;
    QSpinBox* m_threadSpin;
    QSpinBox* m_ramSpin;
    QListWidget* m_reposListWidget;
    QLineEdit* m_newRepoEdit;
};

} // namespace blastro
