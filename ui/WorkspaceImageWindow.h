#pragma once
#include "core/WorkspaceRegistry.h"
#include "ImageView.h"
#include "HistogramWidget.h"
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>

namespace blastro {

class WorkspaceImageWindow : public QWidget {
    Q_OBJECT
public:
    WorkspaceImageWindow(const QString& name, const WorkspaceElement& element, QWidget* parent = nullptr);
    ~WorkspaceImageWindow() override = default;

    ImageVariant currentImage() const;
    ImageVariant originalImage() const;
    ImageView* imageView() const { return m_imageView; }
    WorkspaceElement element() const { return m_element; }
    QWidget* viewportWidget() const { return m_viewportWidget; }

    void updateName(const QString& newName);
    void notifyImageUpdated();
    void setUpdatesSuspended(bool suspended);

    // Live Preview Support
    void setPreviewImage(const ImageVariant& previewImage);
    void restoreOriginalImage();
    void commitPreviewImage(const ImageVariant& finalImage);

signals:
    void renameRequested(const QString& oldName, const QString& newName);

private slots:
    void onModeButtonClicked(int id);
    void onStretchParamsChangedInWidget(double b, double w, double m);
    void onStretchParamsChangedInView(double b, double w, double m);
    void onFrameChanged(int index);
    void onRenameClicked();

private:
    void updateHistogram();

    QString m_name;
    WorkspaceElement m_element;
    
    ImageView* m_imageView;
    QWidget* m_viewportWidget; // ImageView or BatchImageWidget

    ImageVariant m_originalImageForPreview;
    bool m_hasPreviewActive = false;

    // UI elements
    QWidget* m_headerBar;
    QButtonGroup* m_modeGroup;
    QPushButton* m_normalBtn;
    QPushButton* m_stretchBtn;
    QPushButton* m_autoBtn;
    QPushButton* m_localHistBtn;
    QPushButton* m_expandHistBtn;
    QPushButton* m_nameBtn;

    // Collapsible Histogram layout elements
    QWidget* m_headerHistContainer;
    QWidget* m_expandedHistBar;
    bool m_histExpanded = false;

    // Channel selection buttons for RGB Images
    QButtonGroup* m_channelGroup;
    QPushButton* m_rChanBtn;
    QPushButton* m_gChanBtn;
    QPushButton* m_bChanBtn;
    QPushButton* m_rgbChanBtn;

    HistogramWidget* m_histogramWidget;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void onExpandHistClicked();
    void updateNameLabelText();
};

} // namespace blastro
