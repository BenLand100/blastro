#include "BatchImageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>

namespace blastro {

BatchImageWidget::BatchImageWidget(ImageBatchPtr batch, QWidget* parent)
    : QWidget(parent),
      m_batch(batch),
      m_currentIndex(0),
      m_imageView(new ImageView(this)),
      m_comboBox(new QComboBox(this)),
      m_spinBox(new QSpinBox(this)),
      m_prevBtn(new QPushButton("<", this)),
      m_nextBtn(new QPushButton(">", this)),
      m_checkBox(new QCheckBox(this)), // Checkbox with no text caption
      m_infoLabel(new QLabel(this)) {
      
    // Enable keyboard focus so arrow keys and spacebar function correctly
    setFocusPolicy(Qt::StrongFocus);
      
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Add image view
    mainLayout->addWidget(m_imageView, 1);
    
    // Setup controls layout
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setStyleSheet("background-color: #2a2a2a; color: #ffffff; border-top: 1px solid #333;");
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    controlsLayout->setSpacing(8);
    
    m_infoLabel->setText(QString("Frame: %1 / %2").arg(m_currentIndex + 1).arg(m_batch->count()));
    m_infoLabel->setStyleSheet("font-weight: bold; font-family: monospace; font-size: 11px; min-width: 100px;");
    
    // Configure ComboBox showing the true file name
    m_comboBox->setStyleSheet(
        "QComboBox { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 2px 5px; min-width: 250px; font-size: 11px; height: 20px; max-height: 20px; min-height: 20px; }"
        "QComboBox QAbstractItemView { background-color: #3a3a3a; color: #fff; selection-background-color: #007acc; }"
    );
    updateComboBoxItems();
    
    // Configure SpinBox and hide its default vertical arrows
    m_spinBox->setRange(1, m_batch->count());
    m_spinBox->setValue(1);
    m_spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_spinBox->setAlignment(Qt::AlignCenter);
    // Explicitly set height to 20px to match buttons
    m_spinBox->setStyleSheet(
        "QSpinBox { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 0px 2px; width: 35px; "
        "font-weight: bold; font-family: monospace; font-size: 11px; "
        "height: 20px; min-height: 20px; max-height: 20px; }"
    );
    
    // Navigation buttons styling - Locked to exactly 20px height
    QString btnStyle = 
        "QPushButton { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 0px 8px; font-weight: bold; "
        "min-width: 20px; font-size: 11px; height: 20px; min-height: 20px; max-height: 20px; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }";
    m_prevBtn->setStyleSheet(btnStyle);
    m_nextBtn->setStyleSheet(btnStyle);
    
    // CheckBox styling
    m_checkBox->setStyleSheet("QCheckBox { color: #fff; font-size: 11px; height: 20px; min-height: 20px; max-height: 20px; }");
    
    // Layout elements
    controlsLayout->addWidget(m_infoLabel);
    controlsLayout->addWidget(m_comboBox, 1); // Expand combo box to occupy space
    
    // Navigation group (button + spinbox + button)
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setSpacing(2);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->addWidget(m_prevBtn);
    navLayout->addWidget(m_spinBox);
    navLayout->addWidget(m_nextBtn);
    controlsLayout->addLayout(navLayout);
    
    controlsLayout->addWidget(m_checkBox);
    
    mainLayout->addWidget(controlsWidget);
    
    // Connect controls
    connect(m_comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &BatchImageWidget::onIndexChanged);
    
    connect(m_spinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int val) {
        onIndexChanged(val - 1);
    });
    
    connect(m_prevBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex > 0) {
            onIndexChanged(m_currentIndex - 1);
        }
    });
    
    connect(m_nextBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex < m_batch->count() - 1) {
            onIndexChanged(m_currentIndex + 1);
        }
    });
    
    connect(m_checkBox, &QCheckBox::toggled, this, [this](bool checked) {
        m_batch->setFrameSelected(m_currentIndex, checked);
        m_imageView->setFrameSelectedStatus(checked);
        updateComboBoxItems();
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
    
    // Block signals to prevent infinite update loops
    m_comboBox->blockSignals(true);
    m_comboBox->setCurrentIndex(index);
    m_comboBox->blockSignals(false);
    
    m_spinBox->blockSignals(true);
    m_spinBox->setValue(index + 1);
    m_spinBox->blockSignals(false);
    
    m_checkBox->blockSignals(true);
    m_checkBox->setChecked(m_batch->isFrameSelected(index));
    m_checkBox->blockSignals(false);
    
    m_imageView->setFrameSelectedStatus(m_batch->isFrameSelected(index));
    
    m_infoLabel->setText(QString("Frame: %1 / %2").arg(m_currentIndex + 1).arg(m_batch->count()));
    
    try {
        ImageVariant img = m_batch->getImage(m_currentIndex);
        m_imageView->setImage(img);
        emit frameChanged(index);
    } catch (const std::exception& e) {
        m_infoLabel->setText(QString("Error loading frame %1: %2").arg(index + 1).arg(e.what()));
    }
}

void BatchImageWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left) {
        if (m_currentIndex > 0) {
            onIndexChanged(m_currentIndex - 1);
        }
        event->accept();
    } else if (event->key() == Qt::Key_Right) {
        if (m_currentIndex < m_batch->count() - 1) {
            onIndexChanged(m_currentIndex + 1);
        }
        event->accept();
    } else if (event->key() == Qt::Key_Space) {
        bool currentStatus = m_batch->isFrameSelected(m_currentIndex);
        m_batch->setFrameSelected(m_currentIndex, !currentStatus);
        
        m_checkBox->blockSignals(true);
        m_checkBox->setChecked(!currentStatus);
        m_checkBox->blockSignals(false);
        
        m_imageView->setFrameSelectedStatus(!currentStatus);
        updateComboBoxItems();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void BatchImageWidget::addFrame(const std::string& name, const std::string& filepath, ImageVariant image) {
    // Add to core batch using a lambda loader that captures the preloaded image
    m_batch->addFrame(name, filepath, [image]() { return image; });
    
    // Update spinbox range
    m_spinBox->blockSignals(true);
    m_spinBox->setRange(1, m_batch->count());
    m_spinBox->blockSignals(false);
    
    // Update combobox items first so the new item exists when onIndexChanged is called
    updateComboBoxItems();
    
    // Switch to the newly added frame
    onIndexChanged(m_batch->count() - 1);
}

void BatchImageWidget::updateComboBoxItems() {
    m_comboBox->blockSignals(true);
    m_comboBox->clear();
    for (int i = 0; i < m_batch->count(); ++i) {
        QString name = QString::fromStdString(m_batch->frameName(i));
        bool selected = m_batch->isFrameSelected(i);
        if (!selected) {
            name = "[X] " + name;
        }
        m_comboBox->addItem(name);
        if (!selected) {
            m_comboBox->setItemData(i, QColor(180, 80, 80), Qt::ForegroundRole);
        }
    }
    m_comboBox->setCurrentIndex(m_currentIndex);
    m_comboBox->blockSignals(false);
}

} // namespace blastro
