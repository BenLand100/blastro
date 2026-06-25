#include "BatchImageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace blastro {

BatchImageWidget::BatchImageWidget(ImageBatchPtr batch, QWidget* parent)
    : QWidget(parent),
      m_batch(batch),
      m_currentIndex(0),
      m_imageView(new ImageView(this)),
      m_slider(new QSlider(Qt::Horizontal, this)),
      m_spinBox(new QSpinBox(this)),
      m_infoLabel(new QLabel(this)) {
      
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Add image view
    mainLayout->addWidget(m_imageView, 1);
    
    // Setup controls layout
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setStyleSheet("background-color: #2a2a2a; color: #ffffff;");
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    
    m_infoLabel->setText(QString("Frame: %1 / %2").arg(m_currentIndex + 1).arg(m_batch->count()));
    m_infoLabel->setStyleSheet("font-weight: bold; font-family: monospace;");
    
    m_slider->setRange(0, m_batch->count() - 1);
    m_slider->setValue(0);
    m_slider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: #555; border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 14px; background: #007acc; border-radius: 7px; margin: -4px 0; }"
    );
    
    m_spinBox->setRange(1, m_batch->count());
    m_spinBox->setValue(1);
    m_spinBox->setStyleSheet("background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 2px;");
    
    controlsLayout->addWidget(m_infoLabel);
    controlsLayout->addWidget(m_slider, 1);
    controlsLayout->addWidget(m_spinBox);
    
    mainLayout->addWidget(controlsWidget);
    
    // Connect controls
    connect(m_slider, &QSlider::valueChanged, this, [this](int val) {
        m_spinBox->blockSignals(true);
        m_spinBox->setValue(val + 1);
        m_spinBox->blockSignals(false);
        onIndexChanged(val);
    });
    
    connect(m_spinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val) {
        m_slider->blockSignals(true);
        m_slider->setValue(val - 1);
        m_slider->blockSignals(false);
        onIndexChanged(val - 1);
    });
    
    // Initialize with the first image
    onIndexChanged(0);
}

ImageVariant BatchImageWidget::currentImage() const {
    return m_batch->getImage(m_currentIndex);
}

void BatchImageWidget::onIndexChanged(int index) {
    if (index < 0 || index >= m_batch->count()) return;
    
    m_currentIndex = index;
    m_infoLabel->setText(QString("Frame: %1 / %2").arg(m_currentIndex + 1).arg(m_batch->count()));
    
    try {
        ImageVariant img = m_batch->getImage(m_currentIndex);
        m_imageView->setImage(img);
        emit frameChanged(index);
    } catch (const std::exception& e) {
        m_infoLabel->setText(QString("Error loading frame %1: %2").arg(index + 1).arg(e.what()));
    }
}

} // namespace blastro
