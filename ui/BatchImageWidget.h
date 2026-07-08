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
#include "core/ImageBatch.h"
#include "ImageView.h"
#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QKeyEvent>

namespace blastro {

class BatchImageWidget : public QWidget {
    Q_OBJECT
public:
    explicit BatchImageWidget(ImageBatchPtr batch, QWidget* parent = nullptr);
    ~BatchImageWidget() override = default;

    ImageBatchPtr batch() const { return m_batch; }
    int currentIndex() const { return m_currentIndex; }
    ImageVariant currentImage() const;
    ImageView* imageView() const { return m_imageView; }
    void addFrame(const std::string& name, const std::string& filepath, ImageVariant image);
    void notifyBatchUpdated();
    void setCurrentIndex(int index);

signals:
    void frameChanged(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onIndexChanged(int index);
    void updateStarOverlay();

private:
    void updateComboBoxItems();
    void updateBottomBarReadout();

    ImageBatchPtr m_batch;
    int m_currentIndex;

    ImageView* m_imageView;
    QComboBox* m_comboBox;
    QSpinBox* m_spinBox;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QCheckBox* m_checkBox;
    QLabel* m_infoLabel;

    // Bottom parameter/registration bar
    QWidget* m_bottomBar;
    QCheckBox* m_showStarsCheck;
    QCheckBox* m_showConstCheck;
    QLabel* m_dxLabel;
    QLabel* m_dyLabel;
    QLabel* m_thetaLabel;
    QLabel* m_starsLabel;
    QPushButton* m_filterBtn;
    bool m_firstLoad;
};

} // namespace blastro
