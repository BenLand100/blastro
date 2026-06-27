#include "WorkspaceImageWindow.h"
#include "BatchImageWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>

namespace blastro {

WorkspaceImageWindow::WorkspaceImageWindow(const QString& name, const WorkspaceElement& element, QWidget* parent)
    : QWidget(parent),
      m_name(name),
      m_element(element),
      m_imageView(nullptr),
      m_viewportWidget(nullptr),
      m_headerBar(new QWidget(this)),
      m_modeGroup(new QButtonGroup(this)),
      m_normalBtn(new QPushButton("Normal", this)),
      m_stretchBtn(new QPushButton("Manual", this)),
      m_autoBtn(new QPushButton("Auto", this)),
      m_localHistBtn(new QPushButton("Hist", this)),
      m_expandHistBtn(new QPushButton("▼", this)),
      m_headerHistContainer(new QWidget(this)),
      m_expandedHistBar(new QWidget(this)),
      m_channelGroup(new QButtonGroup(this)),
      m_rChanBtn(new QPushButton("R", this)),
      m_gChanBtn(new QPushButton("G", this)),
      m_bChanBtn(new QPushButton("B", this)),
      m_rgbChanBtn(new QPushButton("RGB", this)),
      m_histogramWidget(new HistogramWidget(this)) {

    // 1. Create viewport based on element type
    if (std::holds_alternative<GrayscaleImagePtr>(element) || std::holds_alternative<RGBImagePtr>(element)) {
        ImageView* iv = new ImageView(this);
        if (std::holds_alternative<GrayscaleImagePtr>(element)) {
            iv->setImage(std::get<GrayscaleImagePtr>(element));
        } else {
            iv->setImage(std::get<RGBImagePtr>(element));
        }
        m_imageView = iv;
        m_viewportWidget = iv;
    } else if (std::holds_alternative<ImageBatchPtr>(element)) {
        auto batch = std::get<ImageBatchPtr>(element);
        BatchImageWidget* bw = new BatchImageWidget(batch, this);
        m_imageView = bw->imageView();
        m_viewportWidget = bw;
        connect(bw, &BatchImageWidget::frameChanged, this, &WorkspaceImageWindow::onFrameChanged);
    }

    // 2. Setup layouts
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header bar layout
    m_headerBar->setStyleSheet("background-color: #242424; border-bottom: 1px solid #333;");
    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    headerLayout->setContentsMargins(10, 4, 10, 4);
    headerLayout->setSpacing(0);

    // Expanded Histogram bar layout
    m_expandedHistBar->setStyleSheet("background-color: #1e1e1e; border-bottom: 1px solid #333;");
    m_expandedHistBar->hide();
    QHBoxLayout* expandedLayout = new QHBoxLayout(m_expandedHistBar);
    expandedLayout->setContentsMargins(10, 4, 10, 4);
    expandedLayout->setSpacing(0);

    // Header Histogram container layout
    QHBoxLayout* containerLayout = new QHBoxLayout(m_headerHistContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    containerLayout->addWidget(m_histogramWidget);

    // Segmented button styling for dark theme
    QString segmentedStyle = 
        "QPushButton { background-color: #333; color: #aaa; border: 1px solid #555; padding: 4px 12px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #444; color: #fff; }"
        "QPushButton:checked { background-color: #007acc; color: #fff; border-color: #007acc; }"
        "QPushButton#normal { border-top-left-radius: 4px; border-bottom-left-radius: 4px; border-right: none; }"
        "QPushButton#auto { border-radius: 0px; border-right: none; }"
        "QPushButton#stretch { border-radius: 0px; border-right: none; }"
        "QPushButton#localHist { border-top-right-radius: 4px; border-bottom-right-radius: 4px; }";

    m_normalBtn->setObjectName("normal");
    m_normalBtn->setCheckable(true);
    m_normalBtn->setChecked(true);
    m_autoBtn->setObjectName("auto");
    m_autoBtn->setCheckable(true);
    m_stretchBtn->setObjectName("stretch");
    m_stretchBtn->setCheckable(true);
    m_localHistBtn->setObjectName("localHist");
    m_localHistBtn->setCheckable(true);

    m_normalBtn->setStyleSheet(segmentedStyle);
    m_autoBtn->setStyleSheet(segmentedStyle);
    m_stretchBtn->setStyleSheet(segmentedStyle);
    m_localHistBtn->setStyleSheet(segmentedStyle);

    // Ensure m_autoBtn has a fixed minimum width to contain "Auto H" without shifting layout
    QFontMetrics autoFm(m_autoBtn->font());
    int minW = autoFm.horizontalAdvance("Auto H") + 24;
    m_autoBtn->setMinimumWidth(minW);

    // Order: [Normal] [Auto] [Manual] [Hist]
    m_modeGroup->addButton(m_normalBtn, 0);
    m_modeGroup->addButton(m_autoBtn, 2);
    m_modeGroup->addButton(m_stretchBtn, 1);
    m_modeGroup->addButton(m_localHistBtn, 3);
    m_modeGroup->setExclusive(true);

    headerLayout->addWidget(m_normalBtn);
    headerLayout->addWidget(m_autoBtn);
    headerLayout->addWidget(m_stretchBtn);
    headerLayout->addWidget(m_localHistBtn);

    // Setup Summon/Expand Button
    m_expandHistBtn->setObjectName("expandHist");
    m_expandHistBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #888; font-weight: bold; font-size: 11px; padding: 4px 6px; }"
        "QPushButton:hover { color: #fff; }"
    );
    connect(m_expandHistBtn, &QPushButton::clicked, this, &WorkspaceImageWindow::onExpandHistClicked);

    // Place the HistogramWidget directly in the container, stretching horizontally.
    m_histogramWidget->setActive(false);
    m_histogramWidget->setVisible(true);
    m_histogramWidget->setContentsMargins(0, 0, 0, 0);
    m_histogramWidget->setFixedHeight(24);
    m_histogramWidget->setDrawCurve(false);
    
    // Add spacing and the collapsible widgets to the header
    headerLayout->addSpacing(15);
    headerLayout->addWidget(m_expandHistBtn);
    headerLayout->addWidget(m_headerHistContainer, 1); // Container gets stretch factor 1
    headerLayout->addSpacing(15);

    // Clickable button for name readout (renaming)
    m_nameBtn = new QPushButton("", m_headerBar);
    m_nameBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #888; font-weight: bold; font-family: monospace; font-size: 11px; text-align: right; padding: 0px; }"
        "QPushButton:hover { color: #007acc; text-decoration: underline; }"
    );
    headerLayout->addWidget(m_nameBtn);
    connect(m_nameBtn, &QPushButton::clicked, this, &WorkspaceImageWindow::onRenameClicked);

    // Channel selection styling and configuration for RGB
    bool isRGBElement = false;
    if (std::holds_alternative<RGBImagePtr>(element)) {
        isRGBElement = true;
    } else if (std::holds_alternative<ImageBatchPtr>(element)) {
        auto batch = std::get<ImageBatchPtr>(element);
        if (batch && batch->count() > 0) {
            if (std::holds_alternative<RGBImagePtr>(batch->getImage(0))) {
                isRGBElement = true;
            }
        }
    }

    QString chanStyle = 
        "QPushButton { background-color: #2b2b2b; color: #888; border: 1px solid #444; padding: 2px 6px; font-size: 10px; font-weight: bold; min-width: 20px; }"
        "QPushButton:hover { background-color: #3b3b3b; color: #fff; }"
        "QPushButton:checked { color: #fff; font-weight: bold; }"
        "QPushButton#rgbChan { border-top-left-radius: 3px; border-bottom-left-radius: 3px; border-right: none; }"
        "QPushButton#rgbChan:checked { background-color: #555; border-color: #555; }"
        "QPushButton#rChan { border-radius: 0px; border-right: none; }"
        "QPushButton#rChan:checked { background-color: #a33; border-color: #a33; }"
        "QPushButton#gChan { border-radius: 0px; border-right: none; }"
        "QPushButton#gChan:checked { background-color: #3a3; border-color: #3a3; }"
        "QPushButton#bChan { border-top-right-radius: 3px; border-bottom-right-radius: 3px; }"
        "QPushButton#bChan:checked { background-color: #33a; border-color: #33a; }";

    m_rgbChanBtn->setObjectName("rgbChan");
    m_rgbChanBtn->setCheckable(true);
    m_rgbChanBtn->setChecked(true);
    m_rChanBtn->setObjectName("rChan");
    m_rChanBtn->setCheckable(true);
    m_gChanBtn->setObjectName("gChan");
    m_gChanBtn->setCheckable(true);
    m_bChanBtn->setObjectName("bChan");
    m_bChanBtn->setCheckable(true);

    m_rgbChanBtn->setStyleSheet(chanStyle);
    m_rChanBtn->setStyleSheet(chanStyle);
    m_gChanBtn->setStyleSheet(chanStyle);
    m_bChanBtn->setStyleSheet(chanStyle);

    m_channelGroup->addButton(m_rgbChanBtn, 0);
    m_channelGroup->addButton(m_rChanBtn, 1);
    m_channelGroup->addButton(m_gChanBtn, 2);
    m_channelGroup->addButton(m_bChanBtn, 3);
    m_channelGroup->setExclusive(true);

    if (isRGBElement) {
        headerLayout->addSpacing(8);
        headerLayout->addWidget(m_rgbChanBtn);
        headerLayout->addWidget(m_rChanBtn);
        headerLayout->addWidget(m_gChanBtn);
        headerLayout->addWidget(m_bChanBtn);
    } else {
        m_rgbChanBtn->hide();
        m_rChanBtn->hide();
        m_gChanBtn->hide();
        m_bChanBtn->hide();
    }

    connect(m_channelGroup, qOverload<int>(&QButtonGroup::idClicked), this, [this](int id) {
        if (!m_imageView) return;
        if (id == 0) {
            m_imageView->setChannelMode(ImageView::RGB_ALL);
        } else if (id == 1) {
            m_imageView->setChannelMode(ImageView::RED_ONLY);
        } else if (id == 2) {
            m_imageView->setChannelMode(ImageView::GREEN_ONLY);
        } else if (id == 3) {
            m_imageView->setChannelMode(ImageView::BLUE_ONLY);
        }
    });

    mainLayout->addWidget(m_headerBar);
    mainLayout->addWidget(m_expandedHistBar);

    // Add viewport
    mainLayout->addWidget(m_viewportWidget, 1);

    // Connect signals
    connect(m_modeGroup, qOverload<int>(&QButtonGroup::idClicked), this, &WorkspaceImageWindow::onModeButtonClicked);
    
    // Bidirectional sync between HistogramWidget and ImageView
    connect(m_histogramWidget, &HistogramWidget::stretchParamsChanged, this, &WorkspaceImageWindow::onStretchParamsChangedInWidget);
    connect(m_imageView, &ImageView::stretchParamsChanged, this, &WorkspaceImageWindow::onStretchParamsChangedInView);

    // Initialize histogram
    updateHistogram();
    updateNameLabelText();
}

ImageVariant WorkspaceImageWindow::currentImage() const {
    if (auto iv = qobject_cast<ImageView*>(m_viewportWidget)) {
        return iv->currentImage();
    } else if (auto bw = qobject_cast<BatchImageWidget*>(m_viewportWidget)) {
        return bw->currentImage();
    }
    return ImageVariant();
}

ImageVariant WorkspaceImageWindow::originalImage() const {
    if (m_hasPreviewActive) {
        return m_originalImageForPreview;
    }
    return currentImage();
}

void WorkspaceImageWindow::onModeButtonClicked(int id) {
    if (id == 0) { // Normal
        m_imageView->setDisplayMode(ImageView::Normal);
        m_histogramWidget->setActive(false);
        m_autoBtn->setText("Auto");
        updateHistogram();
    } else if (id == 1) { // Stretch (Manual)
        m_imageView->setDisplayMode(ImageView::Stretch);
        m_histogramWidget->setActive(true);
        m_autoBtn->setText("Auto");
        updateHistogram(); // Refresh histogram display
    } else if (id == 2) { // Auto
        m_imageView->setDisplayMode(ImageView::Autostretch);
        m_histogramWidget->setActive(true);
        int lvl = m_imageView->autoStretchLevel();
        if (lvl == 0) {
            m_autoBtn->setText("Auto L");
        } else if (lvl == 1) {
            m_autoBtn->setText("Auto M");
        } else {
            m_autoBtn->setText("Auto H");
        }
        updateHistogram();
        m_histogramWidget->snapToBlackToMid();
    } else if (id == 3) { // Hist (Local Hist)
        m_imageView->setDisplayMode(ImageView::LocalHist);
        m_histogramWidget->setActive(false);
        m_autoBtn->setText("Auto");
        updateHistogram();
    }
}

void WorkspaceImageWindow::onStretchParamsChangedInWidget(double b, double w, double m) {
    m_imageView->blockSignals(true);
    m_imageView->setStretchParams(b, w, m);
    m_imageView->blockSignals(false);

    if (m_modeGroup->checkedId() == 0 || m_modeGroup->checkedId() == 2 || m_modeGroup->checkedId() == 3) {
        m_modeGroup->button(1)->setChecked(true); // Switch checked button to Stretch
    }
}

void WorkspaceImageWindow::onStretchParamsChangedInView(double b, double w, double m) {
    m_histogramWidget->blockSignals(true);
    m_histogramWidget->setStretchParams(b, w, m);
    m_histogramWidget->blockSignals(false);

    if (m_imageView->displayMode() == ImageView::Autostretch) {
        m_autoBtn->setChecked(true);
        m_histogramWidget->setActive(true);
        int lvl = m_imageView->autoStretchLevel();
        if (lvl == 0) {
            m_autoBtn->setText("Auto L");
        } else if (lvl == 1) {
            m_autoBtn->setText("Auto M");
        } else {
            m_autoBtn->setText("Auto H");
        }
    } else if (m_imageView->displayMode() == ImageView::Stretch) {
        m_stretchBtn->setChecked(true);
        m_histogramWidget->setActive(true);
        m_autoBtn->setText("Auto");
    } else if (m_imageView->displayMode() == ImageView::LocalHist) {
        m_localHistBtn->setChecked(true);
        m_histogramWidget->setActive(false);
        m_autoBtn->setText("Auto");
    } else {
        m_normalBtn->setChecked(true);
        m_histogramWidget->setActive(false);
        m_autoBtn->setText("Auto");
    }
}

void WorkspaceImageWindow::onFrameChanged(int index) {
    Q_UNUSED(index);
    updateHistogram();
}

void WorkspaceImageWindow::updateHistogram() {
    // Query histogram with 16-bit (65536 bins) resolution for high-detail zooming
    m_histogramWidget->setHistogram(m_imageView->getHistogram(65536));
    if (m_histogramWidget->isActive()) {
        m_histogramWidget->setStretchParams(m_imageView->blackpoint(), 
                                            m_imageView->whitepoint(), 
                                            m_imageView->midpoint());
    }
}

void WorkspaceImageWindow::onExpandHistClicked() {
    m_histExpanded = !m_histExpanded;
    QHBoxLayout* headerLayout = qobject_cast<QHBoxLayout*>(m_headerBar->layout());
    if (m_histExpanded) {
        // Move summon button and histogram to expanded bar
        if (headerLayout) {
            headerLayout->removeWidget(m_expandHistBtn);
            headerLayout->setStretchFactor(m_nameBtn, 1);
            headerLayout->setStretchFactor(m_headerHistContainer, 0);
        }
        m_headerHistContainer->layout()->removeWidget(m_histogramWidget);
        m_headerHistContainer->hide();
        
        static_cast<QHBoxLayout*>(m_expandedHistBar->layout())->addWidget(m_expandHistBtn);
        static_cast<QHBoxLayout*>(m_expandedHistBar->layout())->addWidget(m_histogramWidget, 1);
        m_expandedHistBar->show();
        
        m_histogramWidget->setFixedHeight(40);
        m_expandHistBtn->setText("▲");
        
        if (m_nameBtn) {
            m_nameBtn->setMaximumWidth(width() - 250);
        }
    } else {
        // Move back to header
        m_expandedHistBar->layout()->removeWidget(m_expandHistBtn);
        m_expandedHistBar->layout()->removeWidget(m_histogramWidget);
        m_expandedHistBar->hide();
        
        if (headerLayout) {
            int idx = headerLayout->indexOf(m_headerHistContainer);
            if (idx >= 0) {
                headerLayout->insertWidget(idx, m_expandHistBtn);
            } else {
                headerLayout->addWidget(m_expandHistBtn);
            }
            headerLayout->setStretchFactor(m_nameBtn, 0);
            headerLayout->setStretchFactor(m_headerHistContainer, 1);
        }
        
        static_cast<QHBoxLayout*>(m_headerHistContainer->layout())->addWidget(m_histogramWidget);
        m_headerHistContainer->show();
        
        m_histogramWidget->setFixedHeight(24);
        m_expandHistBtn->setText("▼");
        
        if (m_nameBtn) {
            m_nameBtn->setMaximumWidth(width() / 3.0);
        }
    }
    updateNameLabelText();
}

void WorkspaceImageWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_nameBtn) {
        if (m_histExpanded) {
            m_nameBtn->setMaximumWidth(width() - 250);
        } else {
            m_nameBtn->setMaximumWidth(width() / 3.0);
        }
        updateNameLabelText();
    }
}

void WorkspaceImageWindow::updateNameLabelText() {
    if (!m_nameBtn) return;
    int maxAllowedWidth = m_histExpanded ? (width() - 250) : (width() / 3.0);
    maxAllowedWidth = std::max(20, maxAllowedWidth - 10);
    
    QFontMetrics fm(m_nameBtn->font());
    QString elided = fm.elidedText(m_name, Qt::ElideMiddle, maxAllowedWidth);
    m_nameBtn->setText(elided);
    m_nameBtn->setToolTip(m_name);
}

void WorkspaceImageWindow::onRenameClicked() {
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename Workspace Item",
                                          "Enter new unique name:",
                                          QLineEdit::Normal, m_name, &ok);
    if (ok && !newName.trimmed().isEmpty() && newName.trimmed() != m_name) {
        emit renameRequested(m_name, newName.trimmed());
    }
}

void WorkspaceImageWindow::updateName(const QString& newName) {
    m_name = newName;
    updateNameLabelText();
}

void WorkspaceImageWindow::notifyImageUpdated() {
    if (std::holds_alternative<ImageBatchPtr>(m_element)) {
        auto* bw = qobject_cast<BatchImageWidget*>(m_viewportWidget);
        if (bw) {
            bw->notifyBatchUpdated();
        }
    }
    if (m_imageView) {
        if (m_imageView->displayMode() == ImageView::Autostretch) {
            m_imageView->runAutostretch();
        } else {
            m_imageView->setDisplayMode(m_imageView->displayMode());
        }
    }
    updateHistogram();
}

void WorkspaceImageWindow::setPreviewImage(const ImageVariant& previewImage) {
    if (!m_hasPreviewActive) {
        m_originalImageForPreview = currentImage();
        m_hasPreviewActive = true;
    }
    m_imageView->setImage(previewImage, true);
    notifyImageUpdated();
}

void WorkspaceImageWindow::restoreOriginalImage() {
    if (m_hasPreviewActive) {
        m_imageView->setImage(m_originalImageForPreview, true);
        m_hasPreviewActive = false;
        m_originalImageForPreview = ImageVariant();
        notifyImageUpdated();
    }
}

void WorkspaceImageWindow::commitPreviewImage(const ImageVariant& finalImage) {
    if (m_hasPreviewActive) {
        // Mutate the original image in-place
        if (std::holds_alternative<GrayscaleImagePtr>(m_originalImageForPreview) &&
            std::holds_alternative<GrayscaleImagePtr>(finalImage)) {
            auto dst = std::get<GrayscaleImagePtr>(m_originalImageForPreview)->buffer();
            auto src = std::get<GrayscaleImagePtr>(finalImage)->buffer();
            if (dst->width() == src->width() && dst->height() == src->height()) {
                std::copy(src->data(), src->data() + src->width() * src->height(), dst->data());
            }
        } else if (std::holds_alternative<RGBImagePtr>(m_originalImageForPreview) &&
                   std::holds_alternative<RGBImagePtr>(finalImage)) {
            auto dst = std::get<RGBImagePtr>(m_originalImageForPreview);
            auto src = std::get<RGBImagePtr>(finalImage);
            if (dst->width() == src->width() && dst->height() == src->height()) {
                std::copy(src->r()->buffer()->data(), src->r()->buffer()->data() + src->width() * src->height(), dst->r()->buffer()->data());
                std::copy(src->g()->buffer()->data(), src->g()->buffer()->data() + src->width() * src->height(), dst->g()->buffer()->data());
                std::copy(src->b()->buffer()->data(), src->b()->buffer()->data() + src->width() * src->height(), dst->b()->buffer()->data());
            }
        }
        
        // Restore view pointer to the mutated original
        m_imageView->setImage(m_originalImageForPreview, true);
        m_hasPreviewActive = false;
        m_originalImageForPreview = ImageVariant();
        notifyImageUpdated();
    }
}

} // namespace blastro
