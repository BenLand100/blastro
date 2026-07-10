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

#include "AlgorithmDialog.h"
#include <QMdiSubWindow>
#include <QCloseEvent>

namespace blastro {

AlgorithmDialog::AlgorithmDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : QWidget(parent), m_workspace(workspace) {
    // Set centralized high-quality dark theme styling with clear disabled visual indicators
    setStyleSheet(
        "QWidget { background-color: #202020; color: #ffffff; }"
        "QLabel { background-color: transparent; color: #aaaaaa; font-size: 11px; }"
        "QLabel:disabled { color: #4a4a4a; }"
        "QLineEdit { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QLineEdit:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QComboBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QComboBox:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QComboBox QAbstractItemView { background-color: #2a2a2a; color: #ffffff; selection-background-color: #007acc; }"
        "QDoubleSpinBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QDoubleSpinBox:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QSpinBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QSpinBox:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QCheckBox { background-color: transparent; color: #ffffff; font-size: 11px; }"
        "QCheckBox:disabled { color: #4a4a4a; }"
        "QRadioButton { background-color: transparent; color: #ffffff; font-size: 11px; }"
        "QRadioButton:disabled { color: #4a4a4a; }"
        "QSlider::groove:horizontal { border: 1px solid #555555; height: 4px; background: #333333; border-radius: 2px; }"
        "QSlider::groove:horizontal:disabled { background: #161616; border-color: #2c2c2c; }"
        "QSlider::handle:horizontal { background: #007acc; border: 1px solid #555555; width: 12px; margin-top: -4px; margin-bottom: -4px; border-radius: 6px; }"
        "QSlider::handle:horizontal:hover { background: #008be5; }"
        "QSlider::handle:horizontal:disabled { background: #2c2c2c; border-color: #1c1c1c; }"
        "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 5px 14px; border-radius: 3px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }"
        "QPushButton:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QPushButton#primaryButton { background-color: #007acc; border-color: #007acc; color: #ffffff; }"
        "QPushButton#primaryButton:hover { background-color: #008be5; }"
        "QPushButton#primaryButton:pressed { background-color: #0060a0; }"
        "QPushButton#primaryButton:disabled { background-color: #161616; color: #4a4a4a; border-color: #2c2c2c; }"
        "QGroupBox { font-weight: bold; border: 1px solid #555555; margin-top: 10px; padding: 5px; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; }"
        "QGroupBox:disabled { color: #4a4a4a; border-color: #2c2c2c; }"
    );
}

void AlgorithmDialog::onClose() {
    QWidget* p = parentWidget();
    while (p) {
        if (auto sub = qobject_cast<QMdiSubWindow*>(p)) {
            sub->close();
            return;
        }
        p = p->parentWidget();
    }
    close();
}

} // namespace blastro
