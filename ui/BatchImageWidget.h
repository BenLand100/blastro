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

signals:
    void frameChanged(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onIndexChanged(int index);

private:
    void updateComboBoxItems();

    ImageBatchPtr m_batch;
    int m_currentIndex;

    ImageView* m_imageView;
    QComboBox* m_comboBox;
    QSpinBox* m_spinBox;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QCheckBox* m_checkBox;
    QLabel* m_infoLabel;
};

} // namespace blastro
