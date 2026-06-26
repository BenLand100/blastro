#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

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

private:
    QLineEdit* m_moduleEdit;
    QLineEdit* m_libEdit;
    QLineEdit* m_libraryEdit;
    QCheckBox* m_tensorflowChk;
    QLineEdit* m_tempEdit;
    QLineEdit* m_intermediateEdit;
};

} // namespace blastro
