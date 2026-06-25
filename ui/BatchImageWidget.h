#pragma once
#include "core/ImageBatch.h"
#include "ImageView.h"
#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

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

signals:
    void frameChanged(int index);

private slots:
    void onIndexChanged(int index);

private:
    ImageBatchPtr m_batch;
    int m_currentIndex;

    ImageView* m_imageView;
    QSlider* m_slider;
    QSpinBox* m_spinBox;
    QLabel* m_infoLabel;
};

} // namespace blastro
