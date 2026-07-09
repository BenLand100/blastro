/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    void onAddRepo();
    void onRemoveRepo();

private:
    QLineEdit* m_moduleEdit;
    QLineEdit* m_libEdit;
    QLineEdit* m_libraryEdit;
    QCheckBox* m_preloadChk;
    QLineEdit* m_tensorflowUrlEdit;
    QLineEdit* m_tempEdit;
    QLineEdit* m_processFolderEdit;
    QSpinBox* m_threadSpin;
    QSpinBox* m_ramSpin;
    QListWidget* m_reposListWidget;
    QLineEdit* m_newRepoEdit;
};

} // namespace blastro
