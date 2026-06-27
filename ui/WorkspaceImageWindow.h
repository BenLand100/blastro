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
    QPushButton* m_nameBtn;

    HistogramWidget* m_histogramWidget;
};

} // namespace blastro
