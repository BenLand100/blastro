/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "LogWindow.h"
#include "core/PCLBridge.h"
#include "io/QtImageIO.h"
#include "io/FitsIO.h"
#include "algorithms/PixelMathAlgorithm.h"
#include "algorithms/StackingAlgorithm.h"
#include "algorithms/CalibrationAlgorithm.h"
#include "algorithms/DebayerAlgorithm.h"
#include "algorithms/RegisterAlgorithm.h"
#include "algorithms/AlignAlgorithm.h"
#include "algorithms/BackgroundExtractionAlgorithm.h"
#include "algorithms/StretchingAlgorithmWrapper.h"
#include "algorithms/StarFinder.h"
#include "PixelMathDialog.h"
#include "StackingDialog.h"
#include "CalibrationDialog.h"
#include "DebayerDialog.h"
#include "RegisterDialog.h"
#include "AlignDialog.h"
#include "BackgroundExtractionDialog.h"
#include "StretchingDialog.h"
#include "PlatesolveDialog.h"
#include "algorithms/PlatesolveAlgorithm.h"
#include "PreferencesWindow.h"
#include "UpdateManagerDialog.h"
#include "core/TempDirectory.h"
#include "core/Preferences.h"
#include "core/ProjectSerializer.h"
#include "WorkspaceImageWindow.h"
#include "PreprocessingWizardDialog.h"
#include "BatchImageWidget.h"
#include "algorithms/ImageOperations.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDirIterator>
#include <QStatusBar>
#include <QStandardPaths>
#include <QTimer>

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

    // Add status label for left status messages side-by-side with progress bar
    m_statusLabel = new QLabel("Ready", this);
    m_statusLabel->setStyleSheet("color: #aaa; font-family: sans-serif; font-size: 11px; margin-left: 5px; margin-right: 10px;");
    statusBar()->addWidget(m_statusLabel);

    // Add and style progress bar in status bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFixedWidth(150);
    m_progressBar->setFixedHeight(14);
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

    // Print welcome ASCII art and message
    QString welcomeArt = 
        "  ____  _               _             \n"
        " | __ )| |    __ _  ___| |_ _ __ ___  \n"
        " |  _ \\| |   / _` |/ __| __| '__/ _ \\ \n"
        " | |_) | |__| (_| |\\__ \\ |_| | | (_) |\n"
        " |____/|_____\\__,_||___/\\__|_|  \\___/ \n";
    logWindow->appendRawText(welcomeArt, "#00bcd4");
    logWindow->appendRawText(" Welcome to BLastro - Astronomical Image Processing Platform\n"
                             " Version 0.2.0\n"
                             "----------------------------------------------------------------\n", "#a0a0a0");

    // Position log window near the bottom right by default
    logSub->move(0, 500);

    // Connect to subwindow activation signal
    connect(m_workspaceArea, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);
    connect(m_workspaceArea, &WorkspaceArea::elementRenameRequested, this, &MainWindow::onRenameElement);
    connect(m_workspaceArea, &WorkspaceArea::elementClosed, this, [this](const QString& name) {
        m_workspace.unregisterElement(name.toStdString());
    });

    // Auto-load PCL modules from the PCL module folder on startup
    QTimer::singleShot(200, this, [this]() {
        Preferences& prefs = Preferences::instance();
        if (prefs.getPclPreloadLibDir()) {
            PCLBridge::preloadLibraries(QString::fromStdString(prefs.getPclLibFolder()));
        }
        QString moduleFolder = QString::fromStdString(prefs.getPclModuleFolder());
        if (!moduleFolder.isEmpty() && QDir(moduleFolder).exists()) {
            QDirIterator it(moduleFolder, 
#if defined(__PCL_LINUX) || defined(__linux__)
                            QStringList() << "*.so",
#elif defined(__APPLE__)
                            QStringList() << "*.dylib",
#else
                            QStringList() << "*.dll",
#endif
                            QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString fullPath = it.next();
                loadAndShowPlugin(fullPath);
                qDebug() << "[MainWindow] Auto-loaded module:" << it.fileName();
            }
        }
    });

    // Initialise persistent algorithm dialogs (not shown yet, just allocated)
    m_stretchingDlg  = new StretchingDialog(m_workspace, this);
    m_bgeDlg         = new BackgroundExtractionDialog(m_workspace, this);
    m_stackingDlg    = new StackingDialog(m_workspace, this);
    m_registerDlg    = new RegisterDialog(m_workspace, this);
    m_alignDlg       = new AlignDialog(m_workspace, this);
    m_debayerDlg     = new DebayerDialog(m_workspace, this);
    m_calibrationDlg = new CalibrationDialog(m_workspace, this);
    m_pixelMathDlg   = new PixelMathDialog(m_workspace, this);
    m_ppwDlg         = new PreprocessingWizardDialog(m_workspace, this);
    m_platesolveDlg  = new PlatesolveDialog(m_workspace, this);

    // Wire persistent dialog signals
    connect(m_stretchingDlg,  &StretchingDialog::algorithmExecuted,          this, &MainWindow::executeAlgorithmSlot);
    connect(m_bgeDlg,          &BackgroundExtractionDialog::algorithmExecuted, this, &MainWindow::executeAlgorithmSlot);
    connect(m_stackingDlg,     &StackingDialog::algorithmExecuted,            this, &MainWindow::executeAlgorithmSlot);
    connect(m_registerDlg,     &RegisterDialog::algorithmExecuted,            this, &MainWindow::executeAlgorithmSlot);
    connect(m_alignDlg,        &AlignDialog::algorithmExecuted,               this, &MainWindow::executeAlgorithmSlot);
    connect(m_debayerDlg,      &DebayerDialog::algorithmExecuted,             this, &MainWindow::executeAlgorithmSlot);
    connect(m_calibrationDlg,  &CalibrationDialog::algorithmExecuted,        this, &MainWindow::executeAlgorithmSlot);
    connect(m_pixelMathDlg,    &PixelMathDialog::algorithmExecuted,           this, &MainWindow::executeAlgorithmSlot);
    connect(m_platesolveDlg,   &PlatesolveDialog::algorithmExecuted,          this, &MainWindow::executeAlgorithmSlot);

    // Add them to MDI area and hide them initially
    m_stretchingSub  = m_workspaceArea->addSubWindow(m_stretchingDlg);  m_stretchingSub->hide();
    m_bgeSub         = m_workspaceArea->addSubWindow(m_bgeDlg);         m_bgeSub->hide();
    m_stackingSub    = m_workspaceArea->addSubWindow(m_stackingDlg);    m_stackingSub->hide();
    m_registerSub    = m_workspaceArea->addSubWindow(m_registerDlg);    m_registerSub->hide();
    m_alignSub       = m_workspaceArea->addSubWindow(m_alignDlg);       m_alignSub->hide();
    m_debayerSub     = m_workspaceArea->addSubWindow(m_debayerDlg);     m_debayerSub->hide();
    m_calibrationSub = m_workspaceArea->addSubWindow(m_calibrationDlg); m_calibrationSub->hide();
    m_pixelMathSub   = m_workspaceArea->addSubWindow(m_pixelMathDlg);   m_pixelMathSub->hide();
    m_ppwSub         = m_workspaceArea->addSubWindow(m_ppwDlg);         m_ppwSub->hide();
    m_platesolveSub  = m_workspaceArea->addSubWindow(m_platesolveDlg);  m_platesolveSub->hide();

    // Disable WA_DeleteOnClose to keep dialog widgets alive on close
    m_stretchingSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_bgeSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_stackingSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_registerSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_alignSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_debayerSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_calibrationSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_pixelMathSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_ppwSub->setAttribute(Qt::WA_DeleteOnClose, false);
    m_platesolveSub->setAttribute(Qt::WA_DeleteOnClose, false);
}

void MainWindow::createMenus() {
    m_fileMenu = menuBar()->addMenu("&File");
    
    // Image Group
    m_openAct = new QAction("&Open Image", this);
    m_openAct->setShortcut(QKeySequence::Open);
    connect(m_openAct, &QAction::triggered, this, &MainWindow::onOpenImage);
    m_fileMenu->addAction(m_openAct);

    m_saveAct = new QAction("&Save Active Image", this);
    m_saveAct->setShortcut(QKeySequence::Save);
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::onSaveActiveImage);
    m_saveAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_saveAct);

    m_fileMenu->addSeparator();

    // Batch Group
    m_openBatchAct = new QAction("Open &Batch", this);
    connect(m_openBatchAct, &QAction::triggered, this, &MainWindow::onOpenBatch);
    m_fileMenu->addAction(m_openBatchAct);

    m_addToBatchAct = new QAction("&Add to Batch", this);
    connect(m_addToBatchAct, &QAction::triggered, this, &MainWindow::onAddToBatch);
    m_addToBatchAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_addToBatchAct);

    m_saveBatchAct = new QAction("Save Active &Batch", this);
    connect(m_saveBatchAct, &QAction::triggered, this, &MainWindow::onSaveActiveBatch);
    m_saveBatchAct->setEnabled(false); // Grayed out by default
    m_fileMenu->addAction(m_saveBatchAct);

    m_fileMenu->addSeparator();

    // Project Group
    m_newProjectAct = new QAction("New Project", this);
    connect(m_newProjectAct, &QAction::triggered, this, &MainWindow::onNewProject);
    m_fileMenu->addAction(m_newProjectAct);

    m_openProjectAct = new QAction("Open Project", this);
    connect(m_openProjectAct, &QAction::triggered, this, &MainWindow::onOpenProject);
    m_fileMenu->addAction(m_openProjectAct);

    m_saveProjectAct = new QAction("Save Project", this);
    connect(m_saveProjectAct, &QAction::triggered, this, &MainWindow::onSaveProject);
    m_fileMenu->addAction(m_saveProjectAct);

    m_saveProjectAsAct = new QAction("Save Project As...", this);
    connect(m_saveProjectAsAct, &QAction::triggered, this, &MainWindow::onSaveProjectAs);
    m_fileMenu->addAction(m_saveProjectAsAct);

    m_fileMenu->addSeparator();

    m_loadPluginAct = new QAction("&Load PCL Module", this);
    connect(m_loadPluginAct, &QAction::triggered, this, &MainWindow::onLoadPCLModule);
    m_fileMenu->addAction(m_loadPluginAct);

    m_checkForUpdatesAct = new QAction("&PCL Repo Packages", this);
    connect(m_checkForUpdatesAct, &QAction::triggered, this, &MainWindow::onCheckForUpdates);
    m_fileMenu->addAction(m_checkForUpdatesAct);

    m_fileMenu->addSeparator();

    QAction* preferencesAct = new QAction("&Preferences", this);
    connect(preferencesAct, &QAction::triggered, this, &MainWindow::onOpenPreferences);
    m_fileMenu->addAction(preferencesAct);

    m_exitAct = new QAction("E&xit", this);
    connect(m_exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);
    m_fileMenu->addAction(m_exitAct);

    m_imageMenu = menuBar()->addMenu("&Image");
    
    m_undoAct = new QAction("&Undo", this);
    m_undoAct->setShortcut(QKeySequence::Undo);
    connect(m_undoAct, &QAction::triggered, this, &MainWindow::onUndo);
    m_imageMenu->addAction(m_undoAct);

    m_redoAct = new QAction("&Redo", this);
    m_redoAct->setShortcut(QKeySequence::Redo);
    connect(m_redoAct, &QAction::triggered, this, &MainWindow::onRedo);
    m_imageMenu->addAction(m_redoAct);

    m_imageMenu->addSeparator();

    m_flipVertAct = new QAction("Flip &Vertical", this);
    connect(m_flipVertAct, &QAction::triggered, this, &MainWindow::onFlipVertical);
    m_imageMenu->addAction(m_flipVertAct);

    m_mirrorHorizAct = new QAction("Mirror &Horizontal", this);
    connect(m_mirrorHorizAct, &QAction::triggered, this, &MainWindow::onMirrorHorizontal);
    m_imageMenu->addAction(m_mirrorHorizAct);

    m_imageMenu->addSeparator();

    m_rotate90CWAct = new QAction("Rotate 90° &CW", this);
    connect(m_rotate90CWAct, &QAction::triggered, this, &MainWindow::onRotate90CW);
    m_imageMenu->addAction(m_rotate90CWAct);

    m_rotate90CCWAct = new QAction("Rotate 90° C&CW", this);
    connect(m_rotate90CCWAct, &QAction::triggered, this, &MainWindow::onRotate90CCW);
    m_imageMenu->addAction(m_rotate90CCWAct);

    m_rotate180Act = new QAction("Rotate &180°", this);
    connect(m_rotate180Act, &QAction::triggered, this, &MainWindow::onRotate180);
    m_imageMenu->addAction(m_rotate180Act);

    m_imageMenu->addSeparator();

    m_cropAct = new QAction("&Crop", this);
    m_cropAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_K));
    connect(m_cropAct, &QAction::triggered, this, &MainWindow::onCrop);
    m_imageMenu->addAction(m_cropAct);

    m_imageMenu->addSeparator();

    m_showBgeControlPointsAct = new QAction("Show Control Points", this);
    m_showBgeControlPointsAct->setCheckable(true);
    m_showBgeControlPointsAct->setChecked(false);
    connect(m_showBgeControlPointsAct, &QAction::toggled, this, &MainWindow::onToggleShowBgeControlPoints);
    m_imageMenu->addAction(m_showBgeControlPointsAct);

    // Disable all by default
    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    m_flipVertAct->setEnabled(false);
    m_mirrorHorizAct->setEnabled(false);
    m_rotate90CWAct->setEnabled(false);
    m_rotate90CCWAct->setEnabled(false);
    m_rotate180Act->setEnabled(false);
    m_cropAct->setEnabled(false);
    m_showBgeControlPointsAct->setEnabled(false);

    m_algoMenu = menuBar()->addMenu("&Algorithms");
    
    m_calibrationAct = new QAction("&Calibration", this);
    connect(m_calibrationAct, &QAction::triggered, this, &MainWindow::onOpenCalibration);
    m_algoMenu->addAction(m_calibrationAct);

    m_debayerAct = new QAction("&Debayering", this);
    connect(m_debayerAct, &QAction::triggered, this, &MainWindow::onOpenDebayer);
    m_algoMenu->addAction(m_debayerAct);

    m_registerAct = new QAction("Star &Registration", this);
    connect(m_registerAct, &QAction::triggered, this, &MainWindow::onOpenRegister);
    m_algoMenu->addAction(m_registerAct);

    m_alignAct = new QAction("&Alignment", this);
    connect(m_alignAct, &QAction::triggered, this, &MainWindow::onOpenAlign);
    m_algoMenu->addAction(m_alignAct);

    m_stackingAct = new QAction("&Stacking", this);
    connect(m_stackingAct, &QAction::triggered, this, &MainWindow::onOpenStacking);
    m_algoMenu->addAction(m_stackingAct);

    m_platesolveAct = new QAction("Platesolving", this);
    connect(m_platesolveAct, &QAction::triggered, this, &MainWindow::onOpenPlatesolve);
    m_algoMenu->addAction(m_platesolveAct);

    m_algoMenu->addSeparator();

    m_backgroundAct = new QAction("&Background Extraction", this);
    connect(m_backgroundAct, &QAction::triggered, this, &MainWindow::onOpenBackgroundExtraction);
    m_algoMenu->addAction(m_backgroundAct);

    m_stretchAct = new QAction("&Stretching", this);
    connect(m_stretchAct, &QAction::triggered, this, &MainWindow::onOpenGhs);
    m_algoMenu->addAction(m_stretchAct);

    m_pixelMathAct = new QAction("&Pixel Math", this);
    connect(m_pixelMathAct, &QAction::triggered, this, &MainWindow::onOpenPixelMath);
    m_algoMenu->addAction(m_pixelMathAct);

    m_algoMenu->addSeparator();

    // Automation Menu
    QMenu* automationMenu = menuBar()->addMenu("&Automation");
    QAction* wbppAct = new QAction("&Preprocessing Wizard...", this);
    connect(wbppAct, &QAction::triggered, this, &MainWindow::onOpenPpw);
    automationMenu->addAction(wbppAct);

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
    
    // Use filename as the image name by default, resolving duplicates
    QString baseName = info.completeBaseName();
    if (baseName.isEmpty()) {
        baseName = info.fileName();
    }
    QString name = baseName;
    int counter = 1;
    while (m_workspace.contains(name.toStdString())) {
        name = QString("%1_%2").arg(baseName).arg(counter++);
    }

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

    // Use filename as the batch name by default, resolving duplicates
    QString baseName;
    if (selectedFiles.size() == 1) {
        QFileInfo info(selectedFiles.first());
        baseName = info.completeBaseName();
        if (baseName.isEmpty()) baseName = info.fileName();
    } else {
        QFileInfo info(selectedFiles.first());
        baseName = info.completeBaseName() + "_batch";
    }
    QString name = baseName;
    int counter = 1;
    while (m_workspace.contains(name.toStdString())) {
        name = QString("%1_%2").arg(baseName).arg(counter++);
    }

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
    QString defaultPath = activeName;
    if (!defaultPath.endsWith(".fits", Qt::CaseInsensitive) && 
        !defaultPath.endsWith(".fit", Qt::CaseInsensitive) &&
        !defaultPath.endsWith(".png", Qt::CaseInsensitive) &&
        !defaultPath.endsWith(".jpg", Qt::CaseInsensitive) &&
        !defaultPath.endsWith(".jpeg", Qt::CaseInsensitive)) {
        defaultPath += ".fit";
    }
    QString filepath = QFileDialog::getSaveFileName(this, "Save Image As", defaultPath, filter);
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
    m_pixelMathDlg->refreshWorkspaceElements();
    m_pixelMathDlg->show();
    m_pixelMathSub->show();
    m_pixelMathSub->raise();
    m_workspaceArea->setActiveSubWindow(m_pixelMathSub);
}

void MainWindow::onOpenStacking() {
    m_stackingDlg->refreshWorkspaceElements();
    m_stackingDlg->show();
    m_stackingSub->show();
    m_stackingSub->raise();
    m_workspaceArea->setActiveSubWindow(m_stackingSub);
}

void MainWindow::onOpenCalibration() {
    m_calibrationDlg->refreshWorkspaceElements();
    m_calibrationDlg->show();
    m_calibrationSub->show();
    m_calibrationSub->raise();
    m_workspaceArea->setActiveSubWindow(m_calibrationSub);
}

void MainWindow::onOpenPlatesolve() {
    m_platesolveDlg->refreshWorkspaceElements();
    m_platesolveDlg->show();
    m_platesolveSub->show();
    m_platesolveSub->raise();
    m_workspaceArea->setActiveSubWindow(m_platesolveSub);
}

void MainWindow::onOpenDebayer() {
    m_debayerDlg->refreshWorkspaceElements();
    m_debayerDlg->show();
    m_debayerSub->show();
    m_debayerSub->raise();
    m_workspaceArea->setActiveSubWindow(m_debayerSub);
}

void MainWindow::onOpenRegister() {
    m_registerDlg->refreshWorkspaceElements();
    m_registerDlg->show();
    m_registerSub->show();
    m_registerSub->raise();
    m_workspaceArea->setActiveSubWindow(m_registerSub);
}

void MainWindow::onOpenAlign() {
    m_alignDlg->refreshWorkspaceElements();
    m_alignDlg->show();
    m_alignSub->show();
    m_alignSub->raise();
    m_workspaceArea->setActiveSubWindow(m_alignSub);
}

void MainWindow::onOpenBackgroundExtraction() {
    m_bgeDlg->refreshWorkspaceElements();
    m_bgeDlg->show();
    m_bgeSub->show();
    m_bgeSub->raise();
    m_workspaceArea->setActiveSubWindow(m_bgeSub);
}

void MainWindow::onOpenPpw() {
    m_ppwDlg->show();
    m_ppwSub->show();
    m_ppwSub->raise();
    m_workspaceArea->setActiveSubWindow(m_ppwSub);
}

void MainWindow::onOpenGhs() {
    m_stretchingDlg->show();
    m_stretchingSub->show();
    m_stretchingSub->raise();
    m_workspaceArea->setActiveSubWindow(m_stretchingSub);
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
        } else if (m_name == "Debayer") {
            alg = std::make_unique<DebayerAlgorithm>();
        } else if (m_name == "Register") {
            alg = std::make_unique<RegisterAlgorithm>();
        } else if (m_name == "Align") {
            alg = std::make_unique<AlignAlgorithm>();
        } else if (m_name == "BackgroundExtraction") {
            alg = std::make_unique<BackgroundExtractionAlgorithm>();
        } else if (m_name == "Stretching") {
            alg = std::make_unique<StretchingAlgorithmWrapper>();
        } else if (m_name == "Platesolve") {
            alg = std::make_unique<PlatesolveAlgorithm>();
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

void PCLProcessWorker::run() {
    try {
        bool ok = m_bridge->executeProcessInstance(m_processId, m_hProcess, m_buffers);
        emit finished(ok, ok ? "" : "PCL process execution returned failure status.");
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

    // Save session (dialog settings + tool window positions, no image data)
    ProjectSerializer::saveSession(defaultSessionPath(),
                                   m_workspaceArea,
                                   buildDialogSet(),
                                   this);

    m_pclBridge.reset(); // Cleanly unload the PCL module
    TempDirectory::cleanup(); // Clean up temporary directories
    QMainWindow::closeEvent(event);
    QApplication::quit(); // Explicitly terminate the process
}

void MainWindow::executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config) {
    if (m_algorithmRunning) {
        QMessageBox::warning(this, "Algorithm Execution", "Another process or algorithm is currently running. Please wait for it to complete.");
        return;
    }

    std::map<std::string, std::string> resolvedConfig = config;
    if (resolvedConfig.count("output_name")) {
        std::string outName = resolvedConfig.at("output_name");
        if (m_workspace.contains(outName)) {
            std::string base = outName;
            int suffix = 1;
            std::string candidate = base + "_" + std::to_string(suffix);
            while (m_workspace.contains(candidate)) {
                suffix++;
                candidate = base + "_" + std::to_string(suffix);
            }
            resolvedConfig["output_name"] = candidate;
        }
    }

    // Disable menu bar and inner widgets of non-log MDI subwindows to prevent concurrent modifications,
    // while keeping the main window and LogWindow responsive and interactive.
    setProcessingState(true);
    m_algorithmRunning = true;

    // Reset and show progress bar with standard range
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->show();
    showStatusMessage(QString("Running %1...").arg(QString::fromStdString(name)));

    // Create background thread and worker
    QThread* thread = new QThread(this);
    AlgorithmWorker* worker = new AlgorithmWorker(name, resolvedConfig, m_workspace);
    worker->moveToThread(thread);

    // Wire up progress updates
    connect(worker, &AlgorithmWorker::progressUpdated, m_progressBar, &QProgressBar::setValue);

    // Wire up thread start and finish
    connect(thread, &QThread::started, worker, &AlgorithmWorker::run);
    connect(worker, &AlgorithmWorker::finished, this, [this, thread, worker, name, resolvedConfig](bool success, const QString& errorMsg) {
        // Stop thread event loop and wait for completion
        thread->quit();
        thread->wait();

        // Cleanup thread and worker
        worker->deleteLater();
        thread->deleteLater();

        // Reset UI state
        m_progressBar->hide();
        showStatusMessage("Ready");
        m_algorithmRunning = false;

        // Re-enable menu bar and inner widgets of all MDI subwindows
        setProcessingState(false);

        if (success) {
            try {
                if (resolvedConfig.count("input_name")) {
                    QString qInputName = QString::fromStdString(resolvedConfig.at("input_name"));
                    WorkspaceImageWindow* win = m_workspaceArea->getImageWindow(qInputName);
                    if (win) {
                        win->notifyImageUpdated();
                    }
                }

                if (resolvedConfig.count("output_name")) {
                    std::string outName = resolvedConfig.at("output_name");
                    WorkspaceElement outElem = m_workspace.getElement(outName);

                    QString qOutName = QString::fromStdString(outName);
                    WorkspaceImageWindow* win = m_workspaceArea->getImageWindow(qOutName);
                    if (win) {
                        win->saveUndoState();
                        win->setElement(outElem, true); // Update in-place and preserve zoom
                        if (name == "BackgroundExtraction") {
                            if (ImageView* iv = win->imageView()) {
                                iv->setShowBgeControlPoints(false);
                            }
                        }
                    } else {
                        m_workspaceArea->removeElementView(qOutName);
                        m_workspaceArea->addElementView(qOutName, outElem);
                    }

                    showStatusMessage(QString("Successfully completed %1: %2").arg(QString::fromStdString(name)).arg(qOutName), 5000);
                } else {
                    showStatusMessage(QString("Successfully completed %1").arg(QString::fromStdString(name)), 5000);
                }
                
                // Refresh dialogs
                for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                    dlg->refreshWorkspaceElements();
                }
                updateImageMenuState();
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

    // Refresh dialogs
    for (auto* dlg : findChildren<AlgorithmDialog*>()) {
        dlg->refreshWorkspaceElements();
    }
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* window) {
    // Disconnect previous image view mouse tracking if any
    if (m_connectedImageView) {
        disconnect(m_connectedImageView, &ImageView::mousePosChanged, this, &MainWindow::updateStatusReadout);
        disconnect(m_connectedImageView, &ImageView::bgeControlPointsVisibilityChanged, this, &MainWindow::updateImageMenuState);
        m_connectedImageView = nullptr;
    }
    if (m_connectedImageWindow) {
        disconnect(m_connectedImageWindow, &WorkspaceImageWindow::undoRedoStateChanged, this, &MainWindow::updateImageMenuState);
        m_connectedImageWindow = nullptr;
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
            m_connectedImageWindow = wsWindow;
            connect(wsWindow, &WorkspaceImageWindow::undoRedoStateChanged, this, &MainWindow::updateImageMenuState);

            m_connectedImageView = wsWindow->imageView();
            if (m_connectedImageView) {
                connect(m_connectedImageView, &ImageView::mousePosChanged, this, &MainWindow::updateStatusReadout);
                connect(m_connectedImageView, &ImageView::bgeControlPointsVisibilityChanged, this, &MainWindow::updateImageMenuState);
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
    
    // update save menu enable, etc.
    m_saveAct->setEnabled(hasActiveImage && !hasActiveBatch);
    m_addToBatchAct->setEnabled(hasActiveImage && !hasActiveBatch);
    m_saveBatchAct->setEnabled(hasActiveBatch);

    updateImageMenuState();
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

void MainWindow::ensurePCLBridge() {
    if (!m_pclBridge) {
        m_pclBridge = std::make_unique<PCLBridge>(this);
        connect(m_pclBridge.get(), &PCLBridge::progressUpdated, this, [this](int percent) {
            if (m_progressBar) {
                if (m_progressBar->isHidden()) {
                    m_progressBar->setRange(0, 100);
                    m_progressBar->show();
                }
                m_progressBar->setValue(percent);
                if (percent >= 100) {
                    m_progressBar->hide();
                }
            }
        });
    }
}

void MainWindow::setProcessingState(bool processing) {
    menuBar()->setDisabled(processing);
    for (auto* subWin : m_workspaceArea->subWindowList()) {
        auto* widget = subWin->widget();
        if (widget) {
            // Keep Process Console and Preprocessing Wizard fully functional and responsive
            if (qobject_cast<LogWindow*>(widget) || qobject_cast<PreprocessingWizardDialog*>(widget)) {
                continue;
            }
            widget->setDisabled(processing);
            if (auto* imgWin = qobject_cast<WorkspaceImageWindow*>(widget)) {
                imgWin->setUpdatesSuspended(processing);
            }
            
            // Lock or restore subwindow resizing
            if (processing) {
                subWin->setFixedSize(subWin->size());
            } else {
                subWin->setMinimumSize(0, 0);
                subWin->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            }
        }
    }
}

void MainWindow::loadAndShowPlugin(const QString& path) {
    ensurePCLBridge();
    bool success = false;
    if (m_pclBridge->loadModule(path)) {
        success = true;
        auto processes = m_pclBridge->registeredProcesses();
        for (const auto& proc : processes) {
            addPCLProcessToMenu(proc);
        }
        if (processes.empty()) {
            qWarning() << "[MainWindow] No registered processes found in loaded module:" << path;
        }
    } else {
        qWarning() << "[MainWindow] Failed to load module from:" << path;
    }

    if (!success) {
        if (QCoreApplication::arguments().contains("--test-load") || 
            QCoreApplication::arguments().contains("--load-plugin")) {
            qCritical() << "[MainWindow] Plugin load failed. Exiting.";
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

QMdiSubWindow* MainWindow::createPCLPluginSubWindow(QWidget* widget, const QString& processId, const QString& title) {
    QMdiSubWindow* subWindow = createPluginSubWindow(widget, title);
    if (subWindow) {
        m_openPCLInterfaces[processId] = subWindow;
        connect(subWindow, &QObject::destroyed, this, [this, processId]() {
            m_openPCLInterfaces.erase(processId);
        });
    }
    return subWindow;
}

void MainWindow::addPCLProcessToMenu(const QString& processId) {
    if (!m_algoMenu) return;

    // Prevent duplicates
    for (QAction* action : m_algoMenu->actions()) {
        if (action->text() == processId) {
            return;
        }
    }

    // If this is the first PCL process, insert the separator above Load PCL Module
    if (!m_pclSeparatorBelow) {
        m_pclSeparatorBelow = new QAction(this);
        m_pclSeparatorBelow->setSeparator(true);
        m_algoMenu->insertAction(m_loadPluginAct, m_pclSeparatorBelow);
    }

    QAction* act = new QAction(processId, this);
    connect(act, &QAction::triggered, this, [this, processId]() {
        openPCLInterface(processId);
    });
    
    m_algoMenu->insertAction(m_pclSeparatorBelow, act);
}

void MainWindow::openPCLInterface(const QString& processId) {
    auto it = m_openPCLInterfaces.find(processId);
    if (it != m_openPCLInterfaces.end() && it->second) {
        m_workspaceArea->setActiveSubWindow(it->second);
        it->second->show();
        it->second->setFocus();
    } else {
        ensurePCLBridge();
        m_pclBridge->launchInterface(processId, this);
    }
}

void MainWindow::onLoadPCLModule() {
    QString filter = "PCL Modules (*.so);;All Files (*.*)";
    QString filepath = QFileDialog::getOpenFileName(this, "Load PCL Module", "/opt/PixInsight/bin", filter);
    if (filepath.isEmpty()) return;

    ensurePCLBridge();

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
                                 QString("Successfully loaded PCL module:\n%1 (v%2)\n\nRegistered processes:\n%3")
                                 .arg(name, version, procListStr.isEmpty() ? "None" : procListStr));
        
        if (!processes.empty()) {
            for (const auto& proc : processes) {
                addPCLProcessToMenu(proc);
            }
        }
    } else {
        QMessageBox::critical(this, "Load Plugin Error", "Failed to load PCL module. Check console for details.");
    }
}

void MainWindow::onCheckForUpdates() {
    UpdateManagerDialog dlg(this);
    dlg.exec();
}

static void logImageBuffersStatistics(const QString& label, const std::vector<ImageBufferPtr>& bufs) {
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
}

bool MainWindow::executePCLProcessOnActiveImage(const QString& processId, void* hProcess) {
    if (!m_pclBridge) return false;
    if (m_algorithmRunning) {
        QMessageBox::warning(this, "Process Execution", "Another process or algorithm is currently running. Please wait for it to complete.");
        return false;
    }

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

    logImageBuffersStatistics("Before", buffers);

    qDebug() << "[MainWindow] Executing PCL process" << processId << "on active image" << activeName;
    
    setProcessingState(true);
    m_algorithmRunning = true;

    // Save undo state before PCL process mutates the buffers in-place!
    if (auto* win = m_workspaceArea->getImageWindow(activeName)) {
        win->saveUndoState();
    }

    // Reset and show progress bar with standard range
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->show();
    showStatusMessage(QString("Running %1...").arg(processId));

    // Create background thread and worker
    QThread* thread = new QThread(this);
    PCLProcessWorker* worker = new PCLProcessWorker(processId, hProcess, buffers, m_pclBridge.get());
    worker->moveToThread(thread);

    // Connect progress updates
    connect(m_pclBridge.get(), &PCLBridge::progressUpdated, m_progressBar, &QProgressBar::setValue);

    // Wire up thread start and finish
    connect(thread, &QThread::started, worker, &PCLProcessWorker::run);
    connect(worker, &PCLProcessWorker::finished, this, [this, thread, worker, processId, activeName, buffers](bool success, const QString& errorMsg) {
        // Stop thread event loop and wait for completion
        thread->quit();
        thread->wait();

        // Cleanup thread and worker
        worker->deleteLater();
        thread->deleteLater();

        // Reset UI state
        m_progressBar->hide();
        showStatusMessage("Ready");
        m_algorithmRunning = false;

        // Re-enable menu bar and inner widgets of all MDI subwindows
        setProcessingState(false);

        if (success) {
            logImageBuffersStatistics("After", buffers);
            
            // Notify the UI that the image has been modified in-place, preserving stretch settings
            if (auto* win = m_workspaceArea->getImageWindow(activeName)) {
                win->notifyImageUpdated();
            }
            
            showStatusMessage(QString("%1 completed successfully.").arg(processId), 5000);
        } else {
            showStatusMessage(QString("%1 failed.").arg(processId), 5000);
            QMessageBox::critical(this, "Process Execution Error", QString("%1 execution failed. Check console for details. %2").arg(processId, errorMsg));
        }
    });

    thread->start();
    return true;
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
    ensurePCLBridge();

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

    setProcessingState(true);
    bool ok = m_pclBridge->executeProcess(processId, buffers);
    setProcessingState(false);
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
                logWin->show();
                sub->show();
                sub->setFocus();
            }
        });
    } else {
        connect(consoleAct, &QAction::triggered, this, &MainWindow::restoreProcessConsole);
    }
    m_windowMenu->addAction(consoleAct);

    // 2. Preferences
    QAction* prefAct = new QAction("Preferences", this);
    connect(prefAct, &QAction::triggered, this, &MainWindow::onOpenPreferences);
    m_windowMenu->addAction(prefAct);

    m_windowMenu->addSeparator();

    // 3. Cascade/Tile Layouts
    QAction* cascadeAct = new QAction("Cascade", this);
    connect(cascadeAct, &QAction::triggered, m_workspaceArea, &QMdiArea::cascadeSubWindows);
    m_windowMenu->addAction(cascadeAct);

    QAction* tileAct = new QAction("Tile", this);
    connect(tileAct, &QAction::triggered, m_workspaceArea, &QMdiArea::tileSubWindows);
    m_windowMenu->addAction(tileAct);

    // Categorize remaining MDI windows
    QList<QMdiSubWindow*> processes;
    QList<QMdiSubWindow*> images;

    for (auto* subWindow : m_workspaceArea->subWindowList()) {
        if (!subWindow || !subWindow->widget() || !subWindow->isVisible()) continue;
        
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

void MainWindow::onRenameElement(const QString& oldName, const QString& newName) {
    std::string stdOld = oldName.toStdString();
    std::string stdNew = newName.toStdString();
    
    if (m_workspace.contains(stdNew)) {
        QMessageBox::warning(this, "Rename Error",
                             QString("The name '%1' is already in use in the workspace.").arg(newName));
        return;
    }
    
    if (m_workspace.renameElement(stdOld, stdNew)) {
        m_workspaceArea->renameElementView(oldName, newName);
        showStatusMessage(QString("Renamed '%1' to '%2'").arg(oldName, newName), 3000);
        
        // Refresh dialogs
        for (auto* dlg : findChildren<AlgorithmDialog*>()) {
            dlg->refreshWorkspaceElements();
        }
    } else {
        QMessageBox::warning(this, "Rename Error", "Failed to rename workspace element.");
    }
}

void MainWindow::showStatusMessage(const QString& message, int timeout) {
    if (!m_statusLabel) return;
    m_statusLabel->setText(message);
    if (timeout > 0) {
        QTimer::singleShot(timeout, this, [this]() {
            if (m_statusLabel && m_statusLabel->text() != "Ready" && !m_algorithmRunning) {
                m_statusLabel->setText("Ready");
            }
        });
    }
}

void MainWindow::onOpenPreferences() {
    // Check if already open
    for (auto* subWindow : m_workspaceArea->subWindowList()) {
        if (subWindow && subWindow->widget()) {
            if (auto* prefWin = qobject_cast<PreferencesWindow*>(subWindow->widget())) {
                subWindow->show();
                subWindow->setFocus();
                return;
            }
        }
    }

    PreferencesWindow* dlg = new PreferencesWindow(this);
    QMdiSubWindow* sub = m_workspaceArea->addSubWindow(dlg);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    sub->show();
}

void MainWindow::testRegisterOnCube(const QString& cubePath, int refFrameIdx, const QString& detectionMethod) {
    qDebug() << "[MainWindow] Entering testRegisterOnCube. Loading cube:" << cubePath;

    QFileInfo info(cubePath);
    if (!info.exists()) {
        qCritical() << "[MainWindow] FITS cube file does not exist:" << cubePath;
        QCoreApplication::exit(1);
        return;
    }

    ImageBatchPtr batch;
    try {
        FitsIO reader;
        batch = reader.readBatch({cubePath.toStdString()});
    } catch (const std::exception& e) {
        qCritical() << "[MainWindow] Failed to load FITS cube as batch:" << e.what();
        QCoreApplication::exit(1);
        return;
    }

    qDebug() << "[MainWindow] FITS cube loaded successfully as a batch of" << batch->count() << "frames.";
    
    // Analyze baseline star quality in reference frame
    ImageVariant refFrame = batch->getImage(refFrameIdx);
    if (std::holds_alternative<GrayscaleImagePtr>(refFrame)) {
        auto gray = std::get<GrayscaleImagePtr>(refFrame);
        std::vector<Star> refStars = StarFinder::findStars(gray, 100, 5.0, "sota", 10, 1.5, 0.90);
        qDebug() << "[MainWindow] Reference Frame baseline stars found:" << refStars.size();
        double sumFwhm = 0.0;
        double sumEcc = 0.0;
        int count = 0;
        for (const auto& s : refStars) {
            sumFwhm += s.fwhm;
            sumEcc += s.eccentricity;
            count++;
        }
        if (count > 0) {
            qDebug() << "[MainWindow] Reference Frame baseline Avg FWHM:" << (sumFwhm / count) << "pixels";
            qDebug() << "[MainWindow] Reference Frame baseline Avg Eccentricity:" << (sumEcc / count);
        }
    }

    // Register in the workspace
    addImageToWorkspace("LightBatch", batch);

    // Build configuration map for RegisterAlgorithm
    std::map<std::string, std::string> config = {
        {"input_name", "LightBatch"},
        {"ref_frame_index", std::to_string(refFrameIdx)},
        {"detection_method", detectionMethod.toStdString()},
        {"snr_min", "5.0"},          // standard SNR threshold
        {"min_fwhm", "1.5"},
        {"max_stars", "300"},        // plenty of stars for real images
        {"max_eccentricity", "0.90"},
        {"match_tolerance", "3.0"}
    };

    qDebug() << "[MainWindow] Running RegisterAlgorithm on LightBatch...";
    try {
        RegisterAlgorithm reg;
        reg.execute(m_workspace, config, [](int pct) {
            qDebug() << "[MainWindow] Registration progress:" << pct << "%";
        });
        
        qDebug() << "[MainWindow] Registration completed!";
        
        // Print a detailed summary of each frame's registration metadata
        int total = batch->count();
        int registeredCount = 0;
        for (int i = 0; i < total; ++i) {
            FrameMetadata meta = batch->frameMetadata(i);
            bool selected = batch->isFrameSelected(i);
            if (meta.registered) {
                registeredCount++;
                qDebug().noquote() << QString("[Frame %1] SUCCESS: Name=%2, dx=%3, dy=%4, theta=%5, stars=%6, selected=%7")
                    .arg(i)
                    .arg(QString::fromStdString(batch->frameName(i)))
                    .arg(meta.dx)
                    .arg(meta.dy)
                    .arg(meta.theta)
                    .arg(meta.starCount)
                    .arg(selected ? "YES" : "NO");
            } else {
                qDebug().noquote() << QString("[Frame %1] FAILED : Name=%2, selected=%3")
                    .arg(i)
                    .arg(QString::fromStdString(batch->frameName(i)))
                    .arg(selected ? "YES" : "NO");
            }
        }
        qDebug() << "[MainWindow] Summary:" << registeredCount << "/" << total << "frames successfully registered.";
        
        if (registeredCount == 0) {
            qCritical() << "[MainWindow] No frames registered successfully! Aborting alignment.";
            QCoreApplication::exit(1);
            return;
        }

        // Align the registered frames
        qDebug() << "[MainWindow] Running AlignAlgorithm...";
        AlignAlgorithm align;
        align.execute(m_workspace, {
            {"input_name", "LightBatch"},
            {"output_name", "AlignedBatch"},
            {"drizzle_scale", "1.0"},
            {"threads", "-1"},
            {"evict_cache", "true"}
        });

        qDebug() << "[MainWindow] Running StackingAlgorithm...";
        StackingAlgorithm stack;
        stack.execute(m_workspace, {
            {"input_name", "AlignedBatch"},
            {"output_name", "StackedMaster"},
            {"method", "average"},
            {"rejection", "none"},
            {"sigma_low", "3.0"},
            {"sigma_high", "3.0"},
            {"quantile_low", "0.2"},
            {"quantile_high", "0.2"},
            {"stacking_mode", "ram"},
            {"threads", "-1"}
        });

        // Save the stacked result to ./stacked_real_output.fits
        WorkspaceElement stackedElem = m_workspace.getElement("StackedMaster");
        ImageVariant stackedImg;
        if (std::holds_alternative<GrayscaleImagePtr>(stackedElem)) {
            stackedImg = std::get<GrayscaleImagePtr>(stackedElem);
        } else if (std::holds_alternative<RGBImagePtr>(stackedElem)) {
            stackedImg = std::get<RGBImagePtr>(stackedElem);
        }
        
        QString outPath = "./stacked_real_output.fits";
        qDebug() << "[MainWindow] Saving stacked master to:" << outPath;
        FitsIO writer;
        if (writer.writeImage(outPath.toStdString(), stackedImg)) {
            qDebug() << "[MainWindow] Saved stacked master successfully to" << outPath;

            // Save the registered batch to test metadata saving and loading
            QString batchOutPath = "./registered_real_batch.fits";
            qDebug() << "[MainWindow] Saving registered batch to:" << batchOutPath;
            if (writer.writeBatch(batchOutPath.toStdString(), batch)) {
                qDebug() << "[MainWindow] Saved registered batch successfully to" << batchOutPath;
                
                // Read it back to verify persistence!
                try {
                    FitsIO reader;
                    ImageBatchPtr readBackBatch = reader.readBatch({batchOutPath.toStdString()});
                    qDebug() << "[MainWindow] Verified: Read back registered batch with" << readBackBatch->count() << "frames.";
                    int regCount = 0;
                    for (int i = 0; i < readBackBatch->count(); ++i) {
                        if (readBackBatch->frameMetadata(i).registered) regCount++;
                    }
                    qDebug() << "[MainWindow] Verified:" << regCount << "out of" << readBackBatch->count() << "frames have active registration metadata in file.";
                } catch (const std::exception& e) {
                    qCritical() << "[MainWindow] Failed to read back and verify registered batch:" << e.what();
                }
            } else {
                qWarning() << "[MainWindow] Failed to save registered batch.";
            }
            
            // Analyze the stars in the stacked master to evaluate quality
            qDebug() << "[MainWindow] Running StarFinder on stacked master...";
            if (std::holds_alternative<GrayscaleImagePtr>(stackedImg)) {
                auto gray = std::get<GrayscaleImagePtr>(stackedImg);
                std::vector<Star> stars = StarFinder::findStars(gray, 100, 5.0, "sota", 10, 1.5, 0.90);
                qDebug() << "[MainWindow] Found" << stars.size() << "stars in stacked master.";
                
                double sumFwhm = 0.0;
                double sumEcc = 0.0;
                int count = 0;
                for (size_t i = 0; i < stars.size(); ++i) {
                    if (i < 20) {
                        qDebug().noquote() << QString("  Star %1: x=%2, y=%3, peak=%4, fwhm=%5, eccentricity=%6")
                            .arg(i+1)
                            .arg(stars[i].x)
                            .arg(stars[i].y)
                            .arg(stars[i].peak)
                            .arg(stars[i].fwhm)
                            .arg(stars[i].eccentricity);
                    }
                    sumFwhm += stars[i].fwhm;
                    sumEcc += stars[i].eccentricity;
                    count++;
                }
                if (count > 0) {
                    qDebug() << "[MainWindow] Average FWHM:" << (sumFwhm / count) << "pixels";
                    qDebug() << "[MainWindow] Average Eccentricity:" << (sumEcc / count);
                }
            }
            
            QCoreApplication::exit(0);
        } else {
            qCritical() << "[MainWindow] Failed to save stacked master.";
            QCoreApplication::exit(1);
        }
    } catch (const std::exception& e) {
        qCritical() << "[MainWindow] Exception during pipeline execution:" << e.what();
        QCoreApplication::exit(1);
    }
}

void MainWindow::onUndo() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        if (wsWindow->canUndo()) {
            wsWindow->undo();
            QString activeName = wsWindow->windowTitle();
            m_workspace.registerElement(activeName.toStdString(), wsWindow->element());
            for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                dlg->refreshWorkspaceElements();
            }
            updateImageMenuState();
        }
    }
}

void MainWindow::onRedo() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        if (wsWindow->canRedo()) {
            wsWindow->redo();
            QString activeName = wsWindow->windowTitle();
            m_workspace.registerElement(activeName.toStdString(), wsWindow->element());
            for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                dlg->refreshWorkspaceElements();
            }
            updateImageMenuState();
        }
    }
}

void MainWindow::onFlipVertical() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            wsWindow->saveUndoState();
            ImageVariant activeImg = wsWindow->currentImage();
            ImageOperations::flipVertical(activeImg);
            wsWindow->notifyImageUpdated();
            updateImageMenuState();
        }
    }
}

void MainWindow::onMirrorHorizontal() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            wsWindow->saveUndoState();
            ImageVariant activeImg = wsWindow->currentImage();
            ImageOperations::mirrorHorizontal(activeImg);
            wsWindow->notifyImageUpdated();
            updateImageMenuState();
        }
    }
}

void MainWindow::onRotate180() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            wsWindow->saveUndoState();
            ImageVariant activeImg = wsWindow->currentImage();
            ImageOperations::rotate180(activeImg);
            wsWindow->notifyImageUpdated();
            updateImageMenuState();
        }
    }
}

void MainWindow::onRotate90CW() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            wsWindow->saveUndoState();
            ImageVariant activeImg = wsWindow->currentImage();
            ImageVariant rotated = ImageOperations::rotate90CW(activeImg);
            WorkspaceElement elemRotated;
            if (std::holds_alternative<GrayscaleImagePtr>(rotated)) {
                elemRotated = std::get<GrayscaleImagePtr>(rotated);
            } else {
                elemRotated = std::get<RGBImagePtr>(rotated);
            }
            wsWindow->setElement(elemRotated);
            QString activeName = wsWindow->windowTitle();
            m_workspace.registerElement(activeName.toStdString(), elemRotated);
            for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                dlg->refreshWorkspaceElements();
            }
            updateImageMenuState();
        }
    }
}

void MainWindow::onRotate90CCW() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            wsWindow->saveUndoState();
            ImageVariant activeImg = wsWindow->currentImage();
            ImageVariant rotated = ImageOperations::rotate90CCW(activeImg);
            WorkspaceElement elemRotated;
            if (std::holds_alternative<GrayscaleImagePtr>(rotated)) {
                elemRotated = std::get<GrayscaleImagePtr>(rotated);
            } else {
                elemRotated = std::get<RGBImagePtr>(rotated);
            }
            wsWindow->setElement(elemRotated);
            QString activeName = wsWindow->windowTitle();
            m_workspace.registerElement(activeName.toStdString(), elemRotated);
            for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                dlg->refreshWorkspaceElements();
            }
            updateImageMenuState();
        }
    }
}

void MainWindow::onCrop() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) return;
    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
            ImageView* iv = wsWindow->imageView();
            if (iv && iv->hasSelection()) {
                QRect rect = iv->selectionRect();
                wsWindow->saveUndoState();
                ImageVariant activeImg = wsWindow->currentImage();
                ImageVariant cropped = ImageOperations::crop(activeImg, rect);
                WorkspaceElement elemCropped;
                if (std::holds_alternative<GrayscaleImagePtr>(cropped)) {
                    elemCropped = std::get<GrayscaleImagePtr>(cropped);
                } else {
                    elemCropped = std::get<RGBImagePtr>(cropped);
                }
                wsWindow->setElement(elemCropped);
                QString activeName = wsWindow->windowTitle();
                m_workspace.registerElement(activeName.toStdString(), elemCropped);
                iv->clearSelection();
                for (auto* dlg : findChildren<AlgorithmDialog*>()) {
                    dlg->refreshWorkspaceElements();
                }
                updateImageMenuState();
            }
        }
    }
}

void MainWindow::updateImageMenuState() {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (!activeSub) {
        m_undoAct->setEnabled(false);
        m_redoAct->setEnabled(false);
        m_flipVertAct->setEnabled(false);
        m_mirrorHorizAct->setEnabled(false);
        m_rotate90CWAct->setEnabled(false);
        m_rotate90CCWAct->setEnabled(false);
        m_rotate180Act->setEnabled(false);
        m_cropAct->setEnabled(false);
        m_showBgeControlPointsAct->setEnabled(false);
        return;
    }

    if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
        WorkspaceElement elem = wsWindow->element();
        bool isSingleImage = std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem);
        
        m_undoAct->setEnabled(isSingleImage && wsWindow->canUndo());
        m_redoAct->setEnabled(isSingleImage && wsWindow->canRedo());
        
        m_flipVertAct->setEnabled(isSingleImage);
        m_mirrorHorizAct->setEnabled(isSingleImage);
        m_rotate90CWAct->setEnabled(isSingleImage);
        m_rotate90CCWAct->setEnabled(isSingleImage);
        m_rotate180Act->setEnabled(isSingleImage);
        
        ImageView* iv = wsWindow->imageView();
        m_cropAct->setEnabled(isSingleImage && iv && iv->hasSelection());
        m_showBgeControlPointsAct->setEnabled(isSingleImage && iv);
        if (iv) {
            m_showBgeControlPointsAct->blockSignals(true);
            m_showBgeControlPointsAct->setChecked(iv->showBgeControlPoints());
            m_showBgeControlPointsAct->blockSignals(false);
        }
    } else {
        m_undoAct->setEnabled(false);
        m_redoAct->setEnabled(false);
        m_flipVertAct->setEnabled(false);
        m_mirrorHorizAct->setEnabled(false);
        m_rotate90CWAct->setEnabled(false);
        m_rotate90CCWAct->setEnabled(false);
        m_rotate180Act->setEnabled(false);
        m_cropAct->setEnabled(false);
        m_showBgeControlPointsAct->setEnabled(false);
    }
}

void MainWindow::onToggleShowBgeControlPoints(bool checked) {
    QMdiSubWindow* activeSub = m_workspaceArea->activeSubWindow();
    if (activeSub) {
        if (auto* wsWindow = qobject_cast<WorkspaceImageWindow*>(activeSub->widget())) {
            if (ImageView* iv = wsWindow->imageView()) {
                iv->setShowBgeControlPoints(checked, true);
            }
        }
    }
}

// ── Project / session helpers ─────────────────────────────────────────────────

DialogSet MainWindow::buildDialogSet() const {
    DialogSet ds;
    ds.stretching  = m_stretchingDlg;
    ds.bge         = m_bgeDlg;
    ds.stacking    = m_stackingDlg;
    ds.registerDlg = m_registerDlg;
    ds.align       = m_alignDlg;
    ds.debayer     = m_debayerDlg;
    ds.calibration = m_calibrationDlg;
    ds.pixelMath   = m_pixelMathDlg;
    ds.ppw         = m_ppwDlg;
    ds.platesolve  = m_platesolveDlg;
    return ds;
}

QString MainWindow::defaultSessionPath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) configDir = QDir::homePath() + "/.config/BLastro";
    return QDir(configDir).filePath("last_session.json");
}

void MainWindow::applyStartupOptions(const StartupOptions& opts) {
    if (!opts.projectPath.isEmpty()) {
        openProject(opts.projectPath);
    } else if (!opts.sessionPath.isEmpty()) {
        ProjectSerializer::loadSession(opts.sessionPath, m_workspaceArea, buildDialogSet(), this);
    } else if (!opts.noRestore) {
        ProjectSerializer::loadSession(defaultSessionPath(), m_workspaceArea, buildDialogSet(), this);
    }
}

bool MainWindow::openProject(const QString& projectDir) {
    // Change CWD to the project directory
    if (!QDir::setCurrent(projectDir)) {
        QMessageBox::critical(this, "Open Project", "Cannot change working directory to:\n" + projectDir);
        return false;
    }
    m_projectPath = projectDir;
    setWindowTitle(QString("BLastro — %1").arg(QDir(projectDir).dirName()));

    bool ok = ProjectSerializer::loadProject(projectDir,
                                              m_workspace,
                                              m_workspaceArea,
                                              this,
                                              buildDialogSet());
    // Update PPW process folder label
    if (m_ppwDlg) m_ppwDlg->updateProcessDirLabel();
    return ok;
}

bool MainWindow::saveProject() {
    if (m_projectPath.isEmpty()) return saveProjectAs("");
    return ProjectSerializer::saveProject(m_projectPath,
                                          m_workspace,
                                          m_workspaceArea,
                                          buildDialogSet(),
                                          this);
}

bool MainWindow::saveProjectAs(const QString& dir) {
    QString projectDir = dir;
    if (projectDir.isEmpty()) {
        projectDir = QFileDialog::getExistingDirectory(this,
            "Select Project Folder", QDir::currentPath());
    }
    if (projectDir.isEmpty()) return false;

    // Ask about external references
    QStringList external = ProjectSerializer::externalReferences(projectDir, m_workspace);
    if (!external.isEmpty()) {
        auto result = QMessageBox::question(this, "External References",
            QString("The project contains %1 file(s) located outside the project folder.\n"
                    "Would you like to copy them into the project now?\n\n"
                    "(If you choose No, the project will reference them by absolute path.)").
                    arg(external.size()),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (result == QMessageBox::Cancel) return false;
        if (result == QMessageBox::Yes) {
            ProjectSerializer::copyReferencesIntoProject(projectDir, m_workspace);
        }
    }

    QDir::setCurrent(projectDir);
    m_projectPath = projectDir;
    setWindowTitle(QString("BLastro — %1").arg(QDir(projectDir).dirName()));
    if (m_ppwDlg) m_ppwDlg->updateProcessDirLabel();
    return ProjectSerializer::saveProject(projectDir,
                                          m_workspace,
                                          m_workspaceArea,
                                          buildDialogSet(),
                                          this);
}

void MainWindow::onNewProject() {
    QString projectDir = QFileDialog::getExistingDirectory(this,
        "Select New Project Folder", QDir::currentPath());
    if (projectDir.isEmpty()) return;
    QDir::setCurrent(projectDir);
    m_projectPath = projectDir;
    setWindowTitle(QString("BLastro — %1").arg(QDir(projectDir).dirName()));
    if (m_ppwDlg) m_ppwDlg->updateProcessDirLabel();
}

void MainWindow::onOpenProject() {
    QString projectDir = QFileDialog::getExistingDirectory(this,
        "Open Project Folder", QDir::currentPath());
    if (projectDir.isEmpty()) return;
    openProject(projectDir);
}

void MainWindow::onSaveProject() {
    saveProject();
}

void MainWindow::onSaveProjectAs() {
    saveProjectAs("");
}

void MainWindow::loadBatchPaths(const QStringList& paths, const QString& name) {
    if (paths.isEmpty()) return;
    try {
        FitsIO reader;
        std::vector<std::string> names, filepaths;
        ImageBatch::LoaderFunc loader = [paths, &reader](int i) -> ImageVariant {
            return std::visit([](auto&& arg) -> ImageVariant { return arg; },
                              reader.readImage(paths[i].toStdString()));
        };
        for (const auto& p : paths) {
            QFileInfo fi(p);
            names.push_back(fi.completeBaseName().toStdString());
            filepaths.push_back(p.toStdString());
        }
        auto batch = std::make_shared<ImageBatch>(paths.size(), loader, names, filepaths);
        QString wsName = name.isEmpty() ? QFileInfo(paths.first()).completeBaseName() : name;
        addImageToWorkspace(wsName, batch);
    } catch (const std::exception& e) {
        qWarning() << "[MainWindow::loadBatchPaths] Error:" << e.what();
    }
}

} // namespace blastro
