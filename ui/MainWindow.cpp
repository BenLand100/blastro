#include "MainWindow.h"
#include "LogWindow.h"
#include "core/PCLBridge.h"
#include "io/QtImageIO.h"
#include "io/FitsIO.h"
#include "algorithms/PixelMathAlgorithm.h"
#include "algorithms/StackingAlgorithm.h"
#include "algorithms/CalibrationAlgorithm.h"
#include "PixelMathDialog.h"
#include "StackingDialog.h"
#include "CalibrationDialog.h"
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
    setWindowTitle("BLastro - Astronomical Image Processing");
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

    // Add and style progress bar in status bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 1px solid #555;"
        "   background-color: #222;"
        "   color: #fff;"
        "   border-radius: 3px;"
        "   text-align: center;"
        "   font-weight: bold;"
        "   max-height: 14px;"
        "   width: 150px;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #007acc;"
        "}"
    );
    m_progressBar->hide();
    statusBar()->addWidget(m_progressBar);

    // Create the persistent MDI Log window
    LogWindow* logWindow = new LogWindow(this);
    QMdiSubWindow* logSub = m_workspaceArea->addSubWindow(logWindow);
    logSub->setWindowTitle("Process Console");
    logSub->resize(600, 200);
    logSub->show();

    // Position log window near the bottom right by default
    logSub->move(0, 500);

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

    m_stackingAct = new QAction("&Stacking...", this);
    connect(m_stackingAct, &QAction::triggered, this, &MainWindow::onOpenStacking);
    m_algoMenu->addAction(m_stackingAct);

    m_calibrationAct = new QAction("&Calibration...", this);
    connect(m_calibrationAct, &QAction::triggered, this, &MainWindow::onOpenCalibration);
    m_algoMenu->addAction(m_calibrationAct);

    m_algoMenu->addSeparator();

    m_loadPluginAct = new QAction("&Load PixInsight Module...", this);
    connect(m_loadPluginAct, &QAction::triggered, this, &MainWindow::onLoadPCLModule);
    m_algoMenu->addAction(m_loadPluginAct);

    // Window Menu
    m_windowMenu = menuBar()->addMenu("&Window");
    connect(m_windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);
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

void MainWindow::onOpenStacking() {
    StackingDialog* dlg = new StackingDialog(m_workspace, this);
    connect(dlg, &StackingDialog::algorithmExecuted, this, &MainWindow::executeAlgorithmSlot);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void MainWindow::onOpenCalibration() {
    CalibrationDialog* dlg = new CalibrationDialog(m_workspace, this);
    connect(dlg, &CalibrationDialog::algorithmExecuted, this, &MainWindow::executeAlgorithmSlot);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void AlgorithmWorker::run() {
    try {
        std::unique_ptr<Algorithm> alg;
        if (m_name == "PixelMath") {
            alg = std::make_unique<PixelMathAlgorithm>();
        } else if (m_name == "Stacking") {
            alg = std::make_unique<StackingAlgorithm>();
        } else if (m_name == "Calibration") {
            alg = std::make_unique<CalibrationAlgorithm>();
        } else {
            throw std::runtime_error("Unknown algorithm: " + m_name);
        }

        alg->execute(m_workspace, m_config, [this](int percent) {
            emit progressUpdated(percent);
        });

        emit finished(true, "");
    } catch (const std::exception& e) {
        emit finished(false, QString::fromStdString(e.what()));
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_algorithmRunning) {
        QMessageBox::warning(this, "Algorithm Running",
                             "An algorithm is currently running in the background.\n"
                             "Please wait for it to complete before closing the application.");
        event->ignore();
        return;
    }

    // Check for active workspace items if not running in CLI test/load-plugin mode
    bool isCliMode = QCoreApplication::arguments().contains("--test-load") || 
                     QCoreApplication::arguments().contains("--load-plugin");
                     
    if (!isCliMode) {
        bool hasActiveItems = false;
        for (auto* subWindow : m_workspaceArea->subWindowList()) {
            if (subWindow && subWindow->widget()) {
                if (qobject_cast<WorkspaceImageWindow*>(subWindow->widget()) ||
                    qobject_cast<BatchImageWidget*>(subWindow->widget())) {
                    hasActiveItems = true;
                    break;
                }
            }
        }
        
        if (hasActiveItems) {
            auto result = QMessageBox::question(this, "Unsaved Modifications",
                                                 "There are active images in the workspace. Any unsaved modifications will be lost.\n"
                                                 "Are you sure you want to exit?",
                                                 QMessageBox::Yes | QMessageBox::No,
                                                 QMessageBox::No);
            if (result == QMessageBox::No) {
                event->ignore();
                return;
            }
        }
    }

    m_pclBridge.reset(); // Cleanly unload the PCL module
    QMainWindow::closeEvent(event);
    QApplication::quit(); // Explicitly terminate the process
}

void MainWindow::executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config) {
    // Disable main window to prevent concurrent modifications
    setEnabled(false);
    m_algorithmRunning = true;

    // Reset and show progress bar
    m_progressBar->setValue(0);
    m_progressBar->show();
    statusBar()->showMessage(QString("Running %1...").arg(QString::fromStdString(name)));

    // Create background thread and worker
    QThread* thread = new QThread(this);
    AlgorithmWorker* worker = new AlgorithmWorker(name, config, m_workspace);
    worker->moveToThread(thread);

    // Wire up progress updates
    connect(worker, &AlgorithmWorker::progressUpdated, m_progressBar, &QProgressBar::setValue);

    // Wire up thread start and finish
    connect(thread, &QThread::started, worker, &AlgorithmWorker::run);
    connect(worker, &AlgorithmWorker::finished, this, [this, thread, worker, name, config](bool success, const QString& errorMsg) {
        // Stop thread event loop and wait for completion
        thread->quit();
        thread->wait();

        // Cleanup thread and worker
        worker->deleteLater();
        thread->deleteLater();

        // Reset UI state
        m_progressBar->hide();
        statusBar()->showMessage("Ready");
        m_algorithmRunning = false;
        setEnabled(true);

        if (success) {
            try {
                std::string outName = config.at("output_name");
                WorkspaceElement outElem = m_workspace.getElement(outName);

                QString qOutName = QString::fromStdString(outName);
                m_workspaceArea->removeElementView(qOutName);
                m_workspaceArea->addElementView(qOutName, outElem);

                statusBar()->showMessage(QString("Successfully completed %1: %2").arg(QString::fromStdString(name)).arg(qOutName), 5000);
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Display Error", 
                                     QString("Algorithm finished successfully, but failed to retrieve or display the output element:\n%1").arg(e.what()));
            }
        } else {
            QMessageBox::critical(this, QString("%1 Error").arg(QString::fromStdString(name)),
                                 QString("Error during execution:\n%1").arg(errorMsg));
        }
    });

    // Start execution!
    thread->start();
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

void MainWindow::loadAndShowPlugin(const QString& path) {
    if (!m_pclBridge) {
        m_pclBridge = std::make_unique<PCLBridge>();
    }
    bool success = false;
    if (m_pclBridge->loadModule(path)) {
        auto processes = m_pclBridge->registeredProcesses();
        if (!processes.empty()) {
            QString firstProcess = processes[0];
            qDebug() << "[MainWindow] Automatically launching interface for registered process:" << firstProcess;
            success = m_pclBridge->launchInterface(firstProcess, this);
        } else {
            qWarning() << "[MainWindow] No registered processes found in loaded module:" << path;
        }
    } else {
        qWarning() << "[MainWindow] Failed to load module from:" << path;
    }

    if (!success) {
        if (QCoreApplication::arguments().contains("--test-load") || 
            QCoreApplication::arguments().contains("--load-plugin")) {
            qCritical() << "[MainWindow] Plugin load or launch failed. Exiting.";
            std::exit(1);
        }
    }
}

QMdiSubWindow* MainWindow::createPluginSubWindow(QWidget* widget, const QString& title) {
    QMdiSubWindow* subWindow = m_workspaceArea->addSubWindow(widget);
    subWindow->setWindowTitle(title);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->show();
    return subWindow;
}

void MainWindow::onLoadPCLModule() {
    QString filter = "PixInsight Modules (*.so);;All Files (*.*)";
    QString filepath = QFileDialog::getOpenFileName(this, "Load PixInsight Module", "/opt/PixInsight/bin", filter);
    if (filepath.isEmpty()) return;

    if (!m_pclBridge) {
        m_pclBridge = std::make_unique<PCLBridge>();
    }

    if (m_pclBridge->loadModule(filepath)) {
        const api_module_description* desc = m_pclBridge->moduleDescription();
        QString name = QString::fromUtf8(desc->name);
        QString version = QString::fromUtf8(desc->versionInfo);
        
        auto processes = m_pclBridge->registeredProcesses();
        QString procListStr = "";
        for (const auto& proc : processes) {
            if (!procListStr.isEmpty()) procListStr += ", ";
            procListStr += proc;
        }

        QMessageBox::information(this, "Load Plugin",
                                 QString("Successfully loaded PixInsight module:\n%1 (v%2)\n\nRegistered processes:\n%3")
                                 .arg(name, version, procListStr.isEmpty() ? "None" : procListStr));
        
        if (!processes.empty()) {
            qDebug() << "[MainWindow] Automatically launching interface for process:" << processes[0];
            m_pclBridge->launchInterface(processes[0], this);
        }
    } else {
        QMessageBox::critical(this, "Load Plugin Error", "Failed to load PixInsight module. Check console for details.");
    }
}

bool MainWindow::executePCLProcessOnActiveImage(const QString& processId, void* hProcess) {
    if (!m_pclBridge) return false;

    ImageVariant activeImg = m_workspaceArea->getActiveImage();
    QString activeName = m_workspaceArea->getActiveImageName();
    if (activeName.isEmpty()) {
        QMessageBox::warning(this, "Execute Process", "No active image window to process. Open/activate an image first.");
        return false;
    }

    std::vector<ImageBufferPtr> buffers;
    if (std::holds_alternative<GrayscaleImagePtr>(activeImg)) {
        auto img = std::get<GrayscaleImagePtr>(activeImg);
        buffers.push_back(img->buffer());
    } else if (std::holds_alternative<RGBImagePtr>(activeImg)) {
        auto img = std::get<RGBImagePtr>(activeImg);
        buffers.push_back(img->r()->buffer());
        buffers.push_back(img->g()->buffer());
        buffers.push_back(img->b()->buffer());
    }

    if (buffers.empty()) {
        QMessageBox::warning(this, "Execute Process", "No image buffers found in the active window.");
        return false;
    }

    // Print before-statistics for diagnostics
    auto printStats = [](const QString& label, const std::vector<ImageBufferPtr>& bufs) {
        static const char* chanNames[] = {"R","G","B","A"};
        for (int c = 0; c < (int)bufs.size(); ++c) {
            float sum = 0, minV = 1e9f, maxV = -1e9f;
            const float* d = bufs[c]->data();
            int n = bufs[c]->width() * bufs[c]->height();
            for (int p = 0; p < n; ++p) { sum += d[p]; if(d[p]<minV)minV=d[p]; if(d[p]>maxV)maxV=d[p]; }
            qDebug().noquote() << QString("[MainWindow] %1 Ch%2 mean=%3 min=%4 max=%5")
                .arg(label).arg(bufs.size()>1 ? chanNames[c] : "Gray")
                .arg(sum/n, 0, 'f', 6).arg(minV, 0, 'f', 6).arg(maxV, 0, 'f', 6);
        }
    };
    printStats("Before", buffers);

    qDebug() << "[MainWindow] Executing PCL process" << processId << "on active image" << activeName;
    
    statusBar()->showMessage(QString("Running %1...").arg(processId));
    m_progressBar->setRange(0, 0);
    m_progressBar->show();
    qApp->processEvents();

    bool ok = m_pclBridge->executeProcessInstance(processId, hProcess, buffers);

    m_progressBar->hide();
    statusBar()->clearMessage();

    if (ok) {
        printStats("After", buffers);
        WorkspaceElement elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, activeImg);
        m_workspaceArea->removeElementView(activeName);
        m_workspaceArea->addElementView(activeName, elem);
        
        statusBar()->showMessage(QString("%1 completed successfully.").arg(processId), 5000);
        QMessageBox::information(this, "Process Execution", QString("%1 completed successfully.").arg(processId));
    } else {
        QMessageBox::critical(this, "Process Execution Error", QString("%1 execution failed. Check console for details.").arg(processId));
    }

    return ok;
}


void MainWindow::testProcessOnImage(const QString& pluginPath, const QString& imagePath) {
    qDebug() << "[MainWindow] Entering testProcessOnImage. Loading image:" << imagePath;

    QFileInfo info(imagePath);
    if (!info.exists()) {
        qCritical() << "[MainWindow] Image file does not exist:" << imagePath;
        std::_Exit(1);
        return;
    }

    QString ext = info.suffix().toLower();
    WorkspaceElement elem;
    try {
        if (ext == "fits" || ext == "fit") {
            FitsIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(imagePath.toStdString()));
        } else {
            QtImageIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(imagePath.toStdString()));
        }
        addImageToWorkspace("TestImage", elem);
    } catch (const std::exception& e) {
        qCritical() << "[MainWindow] Failed to load image:" << e.what();
        std::_Exit(1);
        return;
    }

    qDebug() << "[MainWindow] Image loaded successfully. Loading PCL module:" << pluginPath;
    if (!m_pclBridge) {
        m_pclBridge = std::make_unique<PCLBridge>();
    }

    if (!m_pclBridge->loadModule(pluginPath)) {
        qCritical() << "[MainWindow] Failed to load PCL module:" << pluginPath;
        std::_Exit(1);
        return;
    }

    auto processes = m_pclBridge->registeredProcesses();
    if (processes.empty()) {
        qCritical() << "[MainWindow] No registered processes found in loaded module.";
        std::_Exit(1);
        return;
    }

    QString processId = processes[0];
    qDebug() << "[MainWindow] Loaded module successfully. Process found:" << processId;

    // Get the buffers of the loaded image
    std::vector<ImageBufferPtr> buffers;
    if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
        auto img = std::get<GrayscaleImagePtr>(elem);
        buffers.push_back(img->buffer());
    } else if (std::holds_alternative<RGBImagePtr>(elem)) {
        auto img = std::get<RGBImagePtr>(elem);
        buffers.push_back(img->r()->buffer());
        buffers.push_back(img->g()->buffer());
        buffers.push_back(img->b()->buffer());
    }

    if (buffers.empty()) {
        qCritical() << "[MainWindow] No image buffers found in loaded image.";
        std::_Exit(1);
        return;
    }

    qDebug() << "[MainWindow] Executing process" << processId << "on loaded image with default settings...";
    bool ok = m_pclBridge->executeProcess(processId, buffers);
    if (ok) {
        qDebug() << "[MainWindow] Process execution completed successfully!";
        
        // Calculate and print statistics for each channel for validation
        if (std::holds_alternative<RGBImagePtr>(elem)) {
            auto rgb = std::get<RGBImagePtr>(elem);
            for (int c = 0; c < 3; ++c) {
                auto chan = (c == 0) ? rgb->r() : ((c == 1) ? rgb->g() : rgb->b());
                float sum = 0.0f;
                float minVal = 1e9f;
                float maxVal = -1e9f;
                const float* data = chan->buffer()->data();
                int numPixels = chan->buffer()->width() * chan->buffer()->height();
                for (int p = 0; p < numPixels; ++p) {
                    float v = data[p];
                    sum += v;
                    if (v < minVal) minVal = v;
                    if (v > maxVal) maxVal = v;
                }
                float mean = sum / numPixels;
                qDebug().noquote() << QString("[MainWindow] Channel %1 statistics: mean = %2, min = %3, max = %4")
                            .arg(c == 0 ? "R" : (c == 1 ? "G" : "B"))
                            .arg(mean).arg(minVal).arg(maxVal);
            }
        } else if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            auto gray = std::get<GrayscaleImagePtr>(elem);
            float sum = 0.0f;
            float minVal = 1e9f;
            float maxVal = -1e9f;
            const float* data = gray->buffer()->data();
            int numPixels = gray->buffer()->width() * gray->buffer()->height();
            for (int p = 0; p < numPixels; ++p) {
                float v = data[p];
                sum += v;
                if (v < minVal) minVal = v;
                if (v > maxVal) maxVal = v;
            }
            float mean = sum / numPixels;
            qDebug().noquote() << QString("[MainWindow] Grayscale statistics: mean = %1, min = %2, max = %3")
                        .arg(mean).arg(minVal).arg(maxVal);
        }
        
        // Save the resulting image to ./test_output.fit
        QString outPath = "./test_output.fit";
        qDebug() << "[MainWindow] Saving denoised image to:" << outPath;
        FitsIO writer;
        ImageVariant imgVar;
        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            imgVar = std::get<GrayscaleImagePtr>(elem);
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            imgVar = std::get<RGBImagePtr>(elem);
        }
        if (writer.writeImage(outPath.toStdString(), imgVar)) {
            qDebug() << "[MainWindow] Saved denoised image successfully.";
        } else {
            qWarning() << "[MainWindow] Failed to save denoised image.";
        }
        
        std::_Exit(0);
    } else {
        qCritical() << "[MainWindow] Process execution failed!";
        std::_Exit(1);
    }
}

bool MainWindow::loadImageDirectly(const QString& filepath, const QString& refName) {
    QFileInfo info(filepath);
    if (!info.exists()) {
        qCritical() << "[MainWindow] Image file does not exist:" << filepath;
        return false;
    }
    QString ext = info.suffix().toLower();
    try {
        WorkspaceElement elem;
        if (ext == "fits" || ext == "fit") {
            FitsIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(filepath.toStdString()));
        } else {
            QtImageIO reader;
            elem = std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, reader.readImage(filepath.toStdString()));
        }
        addImageToWorkspace(refName, elem);
        qDebug() << "[MainWindow] Successfully loaded image" << filepath << "as" << refName;
        return true;
    } catch (const std::exception& e) {
        qCritical() << "[MainWindow] Failed to load image:" << e.what();
        return false;
    }
}

void MainWindow::updateWindowMenu() {
    m_windowMenu->clear();

    // 1. Process Console
    LogWindow* logWin = LogWindow::instance();
    QAction* consoleAct = new QAction("Process Console", this);
    if (logWin) {
        connect(consoleAct, &QAction::triggered, this, [logWin]() {
            if (auto* sub = qobject_cast<QMdiSubWindow*>(logWin->parentWidget())) {
                sub->show();
                sub->setFocus();
            }
        });
    } else {
        connect(consoleAct, &QAction::triggered, this, &MainWindow::restoreProcessConsole);
    }
    m_windowMenu->addAction(consoleAct);

    // Categorize remaining MDI windows
    QList<QMdiSubWindow*> processes;
    QList<QMdiSubWindow*> images;

    for (auto* subWindow : m_workspaceArea->subWindowList()) {
        if (!subWindow || !subWindow->widget()) continue;
        
        // Skip LogWindow itself as it is handled first
        if (qobject_cast<LogWindow*>(subWindow->widget())) continue;

        if (qobject_cast<WorkspaceImageWindow*>(subWindow->widget()) ||
            qobject_cast<BatchImageWidget*>(subWindow->widget())) {
            images.append(subWindow);
        } else {
            processes.append(subWindow);
        }
    }

    // Add separator between builtins (Process Console) and algorithms if algorithms exist
    if (!processes.isEmpty()) {
        m_windowMenu->addSeparator();
        for (auto* sub : processes) {
            QAction* act = new QAction(sub->windowTitle(), this);
            connect(act, &QAction::triggered, this, [sub]() {
                sub->show();
                sub->setFocus();
            });
            m_windowMenu->addAction(act);
        }
    }

    // Add separator before images if images exist and we have added either builtins or algorithms
    if (!images.isEmpty()) {
        m_windowMenu->addSeparator();
        for (auto* sub : images) {
            QAction* act = new QAction(sub->windowTitle(), this);
            connect(act, &QAction::triggered, this, [sub]() {
                sub->show();
                sub->setFocus();
            });
            m_windowMenu->addAction(act);
        }
    }
}

void MainWindow::restoreProcessConsole() {
    LogWindow* logWindow = new LogWindow(this);
    QMdiSubWindow* logSub = m_workspaceArea->addSubWindow(logWindow);
    logSub->setWindowTitle("Process Console");
    logSub->resize(600, 200);
    logSub->show();
    logSub->move(0, 500);
}

} // namespace blastro
