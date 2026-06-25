#include "MainWindow.h"
#include "io/QtImageIO.h"
#include "io/FitsIO.h"
#include "algorithms/PixelMathAlgorithm.h"
#include "PixelMathDialog.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>

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
        "QMenu { background-color: #202020; color: #ffffff; border: 1px solid #555; }"
        "QMenu::item:selected { background-color: #007acc; }"
    );
}

void MainWindow::createMenus() {
    m_fileMenu = menuBar()->addMenu("&File");
    
    m_openAct = new QAction("&Open Image...", this);
    m_openAct->setShortcut(QKeySequence::Open);
    connect(m_openAct, &QAction::triggered, this, &MainWindow::onOpenImage);
    m_fileMenu->addAction(m_openAct);

    m_openBatchAct = new QAction("Open &Batch...", this);
    connect(m_openBatchAct, &QAction::triggered, this, &MainWindow::onOpenBatch);
    m_fileMenu->addAction(m_openBatchAct);

    m_saveAct = new QAction("&Save Active Image...", this);
    m_saveAct->setShortcut(QKeySequence::Save);
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::onSaveActiveImage);
    m_fileMenu->addAction(m_saveAct);

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
    QString filter = "FITS Files (*.fits *.fit);;Standard Images (*.png *.jpg *.jpeg *.tiff *.bmp);;All Files (*.*)";
    QStringList filepaths = QFileDialog::getOpenFileNames(this, "Select Batch Images", "", filter);
    if (filepaths.isEmpty()) return;

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

} // namespace blastro
