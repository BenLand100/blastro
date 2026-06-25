#include "MainWindow.h"
#include "io/QtImageIO.h"
#include "io/FitsIO.h"
#include "algorithms/PixelMathAlgorithm.h"
#include "PixelMathDialog.h"
#include "WorkspaceImageWindow.h"
#include "BatchImageWidget.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStatusBar>

namespace blastro {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_workspaceArea(new WorkspaceArea(this)),
      m_imageCounter(1) {
      
    setCentralWidget(m_workspaceArea);
    setWindowTitle("Blastro - Astronomical Image Processing");
    resize(1024, 768);

    createMenus();

    // Dark theme for main window
    setStyleSheet(
        "QMainWindow { background-color: #121212; }"
        "QMenuBar { background-color: #202020; color: #ffffff; }"
        "QMenuBar::item:selected { background-color: #007acc; }"
        "QMenuBar::item:disabled { color: #555555; }"
        "QMenu { background-color: #202020; color: #ffffff; border: 1px solid #555; }"
        "QMenu::item:selected { background-color: #007acc; color: #ffffff; }"
        "QMenu::item:disabled { color: #555555; background-color: transparent; }"
    );

    // Initialize status bar
    m_statusReadout = new QLabel(this);
    m_statusReadout->setStyleSheet("color: #aaa; font-family: monospace; font-size: 11px; margin-right: 10px;");
    statusBar()->addPermanentWidget(m_statusReadout);
    statusBar()->setStyleSheet("QStatusBar { background-color: #202020; color: #aaa; border-top: 1px solid #333; font-size: 11px; }");
    statusBar()->showMessage("Ready");

    // Connect to subwindow activation signal
    connect(m_workspaceArea, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);
}

void MainWindow::createMenus() {
    m_fileMenu = menuBar()->addMenu("&File");
    
    // Image Group
    m_openAct = new QAction("&Open Image...", this);
    m_openAct->setShortcut(QKeySequence::Open);
    connect(m_openAct, &QAction::triggered, this, &MainWindow::onOpenImage);
    m_fileMenu->addAction(m_openAct);

    m_saveAct = new QAction("&Save Active Image...", this);
    m_saveAct->setShortcut(QKeySequence::Save);
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::onSaveActiveImage);
    m_saveAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_saveAct);

    m_fileMenu->addSeparator();

    // Batch Group
    m_openBatchAct = new QAction("Open &Batch...", this);
    connect(m_openBatchAct, &QAction::triggered, this, &MainWindow::onOpenBatch);
    m_fileMenu->addAction(m_openBatchAct);

    m_addToBatchAct = new QAction("&Add to Batch...", this);
    connect(m_addToBatchAct, &QAction::triggered, this, &MainWindow::onAddToBatch);
    m_addToBatchAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_addToBatchAct);

    m_saveBatchAct = new QAction("Save Active &Batch...", this);
    connect(m_saveBatchAct, &QAction::triggered, this, &MainWindow::onSaveActiveBatch);
    m_saveBatchAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_saveBatchAct);

    m_fileMenu->addSeparator();

    m_exitAct = new QAction("E&xit", this);
    connect(m_exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);
    m_fileMenu->addAction(m_exitAct);

    m_algoMenu = menuBar()->addMenu("&Algorithms");
    
    m_pixelMathAct = new QAction("&Pixel Math...", this);
    connect(m_pixelMathAct, &QAction::triggered, this, &MainWindow::onOpenPixelMath);
    m_algoMenu->addAction(m_pixelMathAct);
}

void MainWindow::onOpenImage() {
    QString filter = "All Images (*.fits *.fit *.png *.jpg *.jpeg *.tiff *.bmp);;FITS Files (*.fits *.fit);;Standard Images (*.png *.jpg *.jpeg *.tiff *.bmp)";
    QString filepath = QFileDialog::getOpenFileName(this, "Open Image", "", filter);
    if (filepath.isEmpty()) return;

    QFileInfo info(filepath);
    QString ext = info.suffix().toLower();
    
    // Prompt for workspace name
    bool ok;
    QString defaultName = QString("Image%1").arg(m_imageCounter++);
    QString name = QInputDialog::getText(this, "Workspace Reference Name",
                                         "Enter unique identifier for this image in workspace:",
                                         QLineEdit::Normal, defaultName, &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    name = name.trimmed();

    try {
        WorkspaceElement elem;
        if (ext == "fits" || ext == "fit") {
            FitsIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(filepath.toStdString()));
        } else {
            QtImageIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(filepath.toStdString()));
        }

        addImageToWorkspace(name, elem);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error Opening Image", QString("Failed to load image:\n%1").arg(e.what()));
    }
}

void MainWindow::onOpenBatch() {
    QString filter = "Batch Formats (*.fits *.fit *.lst);;FITS Files (*.fits *.fit);;File Lists (*.lst);;All Files (*.*)";
    QStringList selectedFiles = QFileDialog::getOpenFileNames(this, "Open Batch or Images", "", filter);
    if (selectedFiles.isEmpty()) return;

    QStringList filepaths;

    // Check if the user selected a single file list file (.lst or .txt)
    if (selectedFiles.size() == 1) {
        QString filepath = selectedFiles.first();
        QFileInfo info(filepath);
        QString ext = info.suffix().toLower();
        if (ext == "lst" || ext == "txt") {
            QFile file(filepath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QDir baseDir = info.dir();
                while (!in.atEnd()) {
                    QString line = in.readLine().trimmed();
                    if (!line.isEmpty()) {
                        QFileInfo lineInfo(line);
                        // Resolve paths relative to the batch list file's directory
                        if (lineInfo.isRelative()) {
                            filepaths.append(baseDir.absoluteFilePath(line));
                        } else {
                            filepaths.append(line);
                        }
                    }
                }
                file.close();
            }
        } else {
            filepaths.append(filepath);
        }
    } else {
        filepaths = selectedFiles;
    }

    if (filepaths.isEmpty()) {
        QMessageBox::warning(this, "Open Batch", "No image files found in the selection.");
        return;
    }

    // Prompt for workspace name
    bool ok;
    QString defaultName = QString("Batch%1").arg(m_imageCounter++);
    QString name = QInputDialog::getText(this, "Workspace Reference Name",
                                         "Enter unique identifier for this batch in workspace:",
                                         QLineEdit::Normal, defaultName, &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    name = name.trimmed();

    std::vector<std::string> stdPaths;
    for (const auto& path : filepaths) {
        stdPaths.push_back(path.toStdString());
    }

    try {
        QFileInfo info(filepaths.first());
        QString ext = info.suffix().toLower();
        
        ImageBatchPtr batch = nullptr;
        if (ext == "fits" || ext == "fit") {
            FitsIO reader;
            batch = reader.readBatch(stdPaths);
        } else {
            QtImageIO reader;
            batch = reader.readBatch(stdPaths);
        }

        if (batch) {
            addImageToWorkspace(name, batch);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error Creating Batch", QString("Failed to load batch:\n%1").arg(e.what()));
    }
}

void MainWindow::onSaveActiveImage() {
    ImageVariant activeImg = m_workspaceArea->getActiveImage();
    QString activeName = m_workspaceArea->getActiveImageName();
    if (activeName.isEmpty()) {
        QMessageBox::warning(this, "Save Image", "No active image window to save");
        return;
    }

    QString filter = "FITS Files (*.fits *.fit);;PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;All Files (*.*)";
    QString filepath = QFileDialog::getSaveFileName(this, "Save Image As", activeName, filter);
    if (filepath.isEmpty()) return;

    QFileInfo info(filepath);
    QString ext = info.suffix().toLower();

    bool success = false;
    try {
        if (ext == "fits" || ext == "fit") {
            FitsIO writer;
            success = writer.writeImage(filepath.toStdString(), activeImg);
        } else {
            QtImageIO writer;
            success = writer.writeImage(filepath.toStdString(), activeImg);
        }

        if (success) {
            QMessageBox::information(this, "Save Image", "Image saved successfully");
        } else {
            QMessageBox::critical(this, "Save Image", "Failed to save image");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Save Image Error", QString("Error saving image:\n%1").arg(e.what()));
    }
}

void MainWindow::onAddToBatch() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) {
        QMessageBox::warning(this, "Add to Batch", "No active window to add to.");
        return;
    }

    QString activeName = m_workspaceArea->getActiveImageName();
    if (activeName.isEmpty()) {
        QMessageBox::warning(this, "Add to Batch", "No active image window.");
        return;
    }

    WorkspaceImageWindow* win = qobject_cast<WorkspaceImageWindow*>(activeSub->widget());
    if (!win) {
        QMessageBox::warning(this, "Add to Batch", "Active window is not an image window.");
        return;
    }

    WorkspaceElement elem = win->element();
    bool wasSingleImage = std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem);
    ImageBatchPtr batch;
    BatchImageWidget* bw = nullptr;

    if (wasSingleImage) {
        // Convert single image to a batch of 1
        ImageVariant image;
        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            image = std::get<GrayscaleImagePtr>(elem);
        } else {
            image = std::get<RGBImagePtr>(elem);
        }

        std::string stdName = activeName.toStdString();
        // Construct the batch
        batch = std::make_shared<ImageBatch>(
            1,
            [image](int) { return image; },
            std::vector<std::string>{ stdName },
            std::vector<std::string>{ "" }
        );

        // Capture geometry of the old window
        QRect geom = activeSub->geometry();

        // Update in the workspace registry and recreate the MDI subwindow
        if (m_workspace.contains(stdName)) {
            m_workspace.unregisterElement(stdName);
        }
        m_workspace.registerElement(stdName, batch);

        m_workspaceArea->removeElementView(activeName);
        QMdiSubWindow* newSub = m_workspaceArea->addElementView(activeName, batch);
        if (newSub) {
            newSub->setGeometry(geom);
            // Re-extract the new WorkspaceImageWindow and its BatchImageWidget
            win = qobject_cast<WorkspaceImageWindow*>(newSub->widget());
            if (win) {
                bw = qobject_cast<BatchImageWidget*>(win->viewportWidget());
            }
        }
        
        // Prompt the user: "Add more images?"
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Add to Batch",
            "This image has been converted to a batch of one.\nDo you want to add more images to this batch?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::No) {
            return; // Done! It stays a batch of one.
        }
    } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
        batch = std::get<ImageBatchPtr>(elem);
        bw = qobject_cast<BatchImageWidget*>(win->viewportWidget());
    }

    if (bw) {
        QString filter = "All Images (*.fits *.fit *.png *.jpg *.jpeg *.tiff *.bmp);;FITS Files (*.fits *.fit);;Standard Images (*.png *.jpg *.jpeg *.tiff *.bmp)";
        QStringList selectedFiles = QFileDialog::getOpenFileNames(this, "Add Images to Batch", "", filter);
        if (selectedFiles.isEmpty()) return;

        for (const auto& filepath : selectedFiles) {
            QFileInfo info(filepath);
            QString ext = info.suffix().toLower();
            std::string stdPath = filepath.toStdString();
            std::string name = info.fileName().toStdString();

            try {
                ImageVariant img;
                if (ext == "fits" || ext == "fit") {
                    FitsIO reader;
                    img = reader.readImage(stdPath);
                } else {
                    QtImageIO reader;
                    img = reader.readImage(stdPath);
                }

                bw->addFrame(name, stdPath, img);
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error Adding Frame",
                                     QString("Failed to load and add frame:\n%1\n\nError: %2").arg(filepath).arg(e.what()));
            }
        }
    } else {
        QMessageBox::warning(this, "Add to Batch", "Could not find batch controls widget.");
    }
}

void MainWindow::onSaveActiveBatch() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) {
        QMessageBox::warning(this, "Save Active Batch", "No active window to save.");
        return;
    }

    WorkspaceImageWindow* win = qobject_cast<WorkspaceImageWindow*>(activeSub->widget());
    if (!win) {
        QMessageBox::warning(this, "Save Active Batch", "Active window is not an image window.");
        return;
    }

    WorkspaceElement elem = win->element();
    if (!std::holds_alternative<ImageBatchPtr>(elem)) {
        QMessageBox::warning(this, "Save Active Batch", "Active window does not contain an image batch.");
        return;
    }

    ImageBatchPtr batch = std::get<ImageBatchPtr>(elem);

    QString filter = "File List (*.lst);;FITS Cube (*.fits *.fit)";
    QString selectedFilter;
    QString filepath = QFileDialog::getSaveFileName(this, "Save Batch As", "", filter, &selectedFilter);
    if (filepath.isEmpty()) return;

    QFileInfo info(filepath);
    QString ext = info.suffix().toLower();

    try {
        bool success = false;
        if (ext == "lst") {
            QFile file(filepath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (int i = 0; i < batch->count(); ++i) {
                    std::string path = batch->frameFilepath(i);
                    if (!path.empty()) {
                        out << QString::fromStdString(path) << "\n";
                    }
                }
                file.close();
                success = true;
            }
        } else if (ext == "fits" || ext == "fit") {
            FitsIO writer;
            success = writer.writeBatch(filepath.toStdString(), batch);
        }

        if (success) {
            QMessageBox::information(this, "Save Batch", "Batch saved successfully.");
        } else {
            QMessageBox::critical(this, "Save Batch", "Failed to save batch.");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Save Batch Error", QString("Error saving batch:\n%1").arg(e.what()));
    }
}

void MainWindow::onOpenPixelMath() {
    PixelMathDialog* dlg = new PixelMathDialog(m_workspace, this);
    connect(dlg, &PixelMathDialog::algorithmExecuted, this, &MainWindow::executeAlgorithmSlot);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void MainWindow::executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config) {
    if (name == "PixelMath") {
        try {
            PixelMathAlgorithm alg;
            alg.execute(m_workspace, config);
            
            std::string outName = config.at("output_name");
            WorkspaceElement outElem = m_workspace.getElement(outName);
            
            // Re-render/display
            QString qOutName = QString::fromStdString(outName);
            m_workspaceArea->removeElementView(qOutName);
            m_workspaceArea->addElementView(qOutName, outElem);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Pixel Math Error", QString("Error during Pixel Math execution:\n%1").arg(e.what()));
        }
    }
}

void MainWindow::addImageToWorkspace(const QString& name, const WorkspaceElement& element) {
    std::string stdName = name.toStdString();
    
    // Register in workspace core model
    if (m_workspace.contains(stdName)) {
        m_workspace.unregisterElement(stdName);
    }
    m_workspace.registerElement(stdName, element);
    
    // Add to MDI workspace area
    m_workspaceArea->removeElementView(name);
    m_workspaceArea->addElementView(name, element);
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* window) {
    // Disconnect previous image view mouse tracking if any
    if (m_connectedImageView) {
        disconnect(m_connectedImageView, &ImageView::mousePosChanged, this, &MainWindow::updateStatusReadout);
        m_connectedImageView = nullptr;
    }

    bool hasActiveImage = false;
    bool hasActiveBatch = false;

    if (!window) {
        if (m_statusReadout) {
            m_statusReadout->clear();
        }
    } else {
        // Get the widget inside the subwindow
        QWidget* widget = window->widget();
        if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(widget)) {
            m_connectedImageView = wsWindow->imageView();
            if (m_connectedImageView) {
                connect(m_connectedImageView, &ImageView::mousePosChanged, this, &MainWindow::updateStatusReadout);
            }
            
            WorkspaceElement elem = wsWindow->element();
            if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
                hasActiveImage = true;
            } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
                hasActiveImage = true;
                hasActiveBatch = true;
            }
        }
    }

    m_saveAct->setEnabled(hasActiveImage);
    m_addToBatchAct->setEnabled(hasActiveImage);
    m_saveBatchAct->setEnabled(hasActiveBatch);
}

void MainWindow::updateStatusReadout(int x, int y, bool isRGB, const std::vector<float>& values) {
    if (!m_statusReadout) return;

    if (x < 0 || y < 0 || values.empty()) {
        m_statusReadout->clear();
        return;
    }

    QString text = QString("X: %1, Y: %2 | ").arg(x).arg(y);
    if (isRGB) {
        if (values.size() >= 3) {
            text += QString("R: %1, G: %2, B: %3")
                        .arg(values[0], 0, 'f', 4)
                        .arg(values[1], 0, 'f', 4)
                        .arg(values[2], 0, 'f', 4);
        }
    } else {
        text += QString("K: %1").arg(values[0], 0, 'f', 4);
    }
    m_statusReadout->setText(text);
}

} // namespace blastro
