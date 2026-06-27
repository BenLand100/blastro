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
      m_stretchBtn(new QPushButton("Stretch", this)),
      m_autoBtn(new QPushButton("Auto", this)),
      m_localHistBtn(new QPushButton("Local Hist", this)),
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

    // Segmented button styling for dark theme
    QString segmentedStyle = 
        "QPushButton { background-color: #333; color: #aaa; border: 1px solid #555; padding: 4px 12px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #444; color: #fff; }"
        "QPushButton:checked { background-color: #007acc; color: #fff; border-color: #007acc; }"
        "QPushButton#normal { border-top-left-radius: 4px; border-bottom-left-radius: 4px; border-right: none; }"
        "QPushButton#stretch { border-radius: 0px; border-right: none; }"
        "QPushButton#auto { border-radius: 0px; border-right: none; }"
        "QPushButton#localHist { border-top-right-radius: 4px; border-bottom-right-radius: 4px; }";

    m_normalBtn->setObjectName("normal");
    m_normalBtn->setCheckable(true);
    m_normalBtn->setChecked(true);
    m_stretchBtn->setObjectName("stretch");
    m_stretchBtn->setCheckable(true);
    m_autoBtn->setObjectName("auto");
    m_autoBtn->setCheckable(true);
    m_localHistBtn->setObjectName("localHist");
    m_localHistBtn->setCheckable(true);

    m_normalBtn->setStyleSheet(segmentedStyle);
    m_stretchBtn->setStyleSheet(segmentedStyle);
    m_autoBtn->setStyleSheet(segmentedStyle);
    m_localHistBtn->setStyleSheet(segmentedStyle);

    m_modeGroup->addButton(m_normalBtn, 0);
    m_modeGroup->addButton(m_stretchBtn, 1);
    m_modeGroup->addButton(m_autoBtn, 2);
    m_modeGroup->addButton(m_localHistBtn, 3);
    m_modeGroup->setExclusive(true);

    headerLayout->addWidget(m_normalBtn);
    headerLayout->addWidget(m_stretchBtn);
    headerLayout->addWidget(m_autoBtn);
    headerLayout->addWidget(m_localHistBtn);

    // Place the HistogramWidget directly in the header, stretching horizontally.
    // It is kept visible always to act as a layout-stable empty spacer in Normal mode.
    m_histogramWidget->setActive(false);
    m_histogramWidget->setVisible(true);
    m_histogramWidget->setContentsMargins(10, 0, 10, 0);
    m_histogramWidget->setFixedHeight(24);
    m_histogramWidget->setDrawCurve(false);
    
    // Add some spacing before the histogram
    headerLayout->addSpacing(15);
    headerLayout->addWidget(m_histogramWidget, 1); // Stretch factor 1
    headerLayout->addSpacing(15);

    // Clickable button for name readout (renaming)
    m_nameBtn = new QPushButton(name, m_headerBar);
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

    // Add viewport
    mainLayout->addWidget(m_viewportWidget, 1);

    // Connect signals
    connect(m_modeGroup, qOverload<int>(&QButtonGroup::idClicked), this, &WorkspaceImageWindow::onModeButtonClicked);
    
    // Bidirectional sync between HistogramWidget and ImageView
    connect(m_histogramWidget, &HistogramWidget::stretchParamsChanged, this, &WorkspaceImageWindow::onStretchParamsChangedInWidget);
    connect(m_imageView, &ImageView::stretchParamsChanged, this, &WorkspaceImageWindow::onStretchParamsChangedInView);

    // Initialize histogram
    updateHistogram();
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
    } else if (id == 1) { // Stretch
        m_imageView->setDisplayMode(ImageView::Stretch);
        m_histogramWidget->setActive(true);
        updateHistogram(); // Refresh histogram display
    } else if (id == 2) { // Auto
        m_imageView->setDisplayMode(ImageView::Autostretch);
        m_histogramWidget->setActive(true);
        updateHistogram();
    } else if (id == 3) { // Local Hist
        m_imageView->setDisplayMode(ImageView::LocalHist);
        m_histogramWidget->setActive(false);
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
    } else if (m_imageView->displayMode() == ImageView::Stretch) {
        m_stretchBtn->setChecked(true);
        m_histogramWidget->setActive(true);
    } else if (m_imageView->displayMode() == ImageView::LocalHist) {
        m_localHistBtn->setChecked(true);
        m_histogramWidget->setActive(false);
    } else {
        m_normalBtn->setChecked(true);
        m_histogramWidget->setActive(false);
    }
}

void WorkspaceImageWindow::onFrameChanged(int index) {
    Q_UNUSED(index);
    updateHistogram();
}

void WorkspaceImageWindow::updateHistogram() {
    if (m_histogramWidget->isActive()) {
        // Query histogram with 16-bit (65536 bins) resolution for high-detail zooming
        m_histogramWidget->setHistogram(m_imageView->getHistogram(65536));
        m_histogramWidget->setStretchParams(m_imageView->blackpoint(), 
                                            m_imageView->whitepoint(), 
                                            m_imageView->midpoint());
    }
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
    if (m_nameBtn) {
        m_nameBtn->setText(newName);
    }
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
