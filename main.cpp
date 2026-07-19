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

#include "ui/MainWindow.h"
#include "ui/LogWindow.h"
#include "core/PCLBridge.h"
#include <QApplication>
#include <QIcon>
#include <QTimer>
#include <QDebug>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <cstring>
#include <iostream>

void blastroMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    // Filter out harmless internal Qt/KDE file dialog warnings to keep logs clean
    if (msg.contains("No node found for item that was just removed")) {
        return;
    }

    // Write to standard error / standard output first (preserves terminal logs)
    std::string txt = msg.toStdString();
    switch (type) {
        case QtDebugMsg:
            std::cerr << "[DEBUG] " << txt << std::endl;
            break;
        case QtInfoMsg:
            std::cerr << "[INFO] " << txt << std::endl;
            break;
        case QtWarningMsg:
            std::cerr << "[WARNING] " << txt << std::endl;
            break;
        case QtCriticalMsg:
            std::cerr << "[CRITICAL] " << txt << std::endl;
            break;
        case QtFatalMsg:
            std::cerr << "[FATAL] " << txt << std::endl;
            break;
    }

    // Forward to on-screen LogWindow if registered
    blastro::LogWindow::appendMessage(type, msg);

    if (type == QtFatalMsg) {
        std::abort();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/bl_spacey_icon.png"));

    QCommandLineParser parser;
    parser.setApplicationDescription("BLastro - Astronomical Image Processing Software");
    parser.addHelpOption();

    QCommandLineOption noRestoreOption("no-restore", "Do not restore the last session on startup.");
    parser.addOption(noRestoreOption);

    QCommandLineOption projectOption("project", "Open a specific project file on startup.", "project_path");
    parser.addOption(projectOption);

    QCommandLineOption sessionOption("session", "Open a specific session file on startup.", "session_path");
    parser.addOption(sessionOption);

    QCommandLineOption testProcessOption("test-process", "Run a test process/plugin on an image and launch GUI.", "plugin_path");
    parser.addOption(testProcessOption);

    QCommandLineOption runPluginOption("run-plugin", "Alias for --test-process.", "plugin_path");
    parser.addOption(runPluginOption);

    QCommandLineOption testRegisterOption("test-register", "Run a registration test on a cube.", "cube_path");
    parser.addOption(testRegisterOption);

    QCommandLineOption testLoadOption("test-load", "Load a PCL module/plugin on startup in GUI mode.", "plugin_path");
    parser.addOption(testLoadOption);

    QCommandLineOption loadPluginOption("load-plugin", "Alias for --test-load.", "plugin_path");
    parser.addOption(loadPluginOption);

    QCommandLineOption refIdxOption("ref-idx", "Reference frame index for registration test (default: 0).", "index");
    parser.addOption(refIdxOption);

    QCommandLineOption methodOption("method", "Alignment method for registration test (default: centroid).", "method");
    parser.addOption(methodOption);

    QCommandLineOption loadImageOption("load-image", "Load the specified image file on startup.", "image_path");
    parser.addOption(loadImageOption);

    parser.addPositionalArgument("images", "Image files to load on startup, or input files for test runs.", "[images...]");

    parser.process(app);

    bool isTestProcess = parser.isSet(testProcessOption) || parser.isSet(runPluginOption);
    bool isTestRegister = parser.isSet(testRegisterOption);
    bool isTestLoad = parser.isSet(testLoadOption) || parser.isSet(loadPluginOption);

    if (isTestProcess) {
        QString pluginPath = parser.isSet(testProcessOption) ? parser.value(testProcessOption) : parser.value(runPluginOption);
        QString imagePath;
        if (parser.positionalArguments().size() > 0) {
            imagePath = parser.positionalArguments().first();
        }
        qDebug() << "Test process execution: plugin =" << pluginPath << "image =" << imagePath;

        blastro::MainWindow w;
        w.show();

        QTimer::singleShot(100, [&w, pluginPath, imagePath]() {
            w.testProcessOnImage(pluginPath, imagePath);
        });

        return app.exec();
    }

    if (isTestRegister) {
        QString cubePath = parser.value(testRegisterOption);
        int refIdx = 0;
        QString method = "centroid";

        if (parser.isSet(refIdxOption)) {
            refIdx = parser.value(refIdxOption).toInt();
        } else if (parser.positionalArguments().size() > 0) {
            refIdx = parser.positionalArguments().at(0).toInt();
        }

        if (parser.isSet(methodOption)) {
            method = parser.value(methodOption);
        } else if (parser.positionalArguments().size() > 1) {
            method = parser.positionalArguments().at(1);
        }

        qDebug() << "Test registration execution: cube =" << cubePath << "refIdx =" << refIdx << "method =" << method;

        auto* w = new blastro::MainWindow();
        w->show();

        // Failsafe timeout after 180 seconds (3 minutes) to ensure process exits even if hung
        QTimer::singleShot(180000, []() {
            qCritical() << "[main] Failsafe timeout triggered! Force exiting...";
            std::_Exit(1);
        });

        QTimer::singleShot(100, [w, cubePath, refIdx, method]() {
            w->testRegisterOnCube(cubePath, refIdx, method);
        });

        int ret = app.exec();
        std::_Exit(ret);
    }

    if (isTestLoad) {
        QString path = parser.isSet(testLoadOption) ? parser.value(testLoadOption) : parser.value(loadPluginOption);
        qDebug() << "Test loading PCL module from:" << path << "in GUI mode";

        blastro::MainWindow w;
        w.show();

        QTimer::singleShot(100, [&w, path]() {
            w.loadAndShowPlugin(path);
        });

        return app.exec();
    }

    blastro::StartupOptions startupOpts;
    startupOpts.noRestore = parser.isSet(noRestoreOption);
    if (parser.isSet(projectOption)) {
        startupOpts.projectPath = parser.value(projectOption);
    }
    if (parser.isSet(sessionOption)) {
        startupOpts.sessionPath = parser.value(sessionOption);
    }

    QStringList positionalImages = parser.positionalArguments();
    if (parser.isSet(loadImageOption)) {
        positionalImages.append(parser.value(loadImageOption));
    }

    QString loadPluginPath;
    if (parser.isSet(loadPluginOption)) {
        loadPluginPath = parser.value(loadPluginOption);
    } else if (parser.isSet(testLoadOption)) {
        loadPluginPath = parser.value(testLoadOption);
    }

    blastro::MainWindow w;
    w.show();

    QTimer::singleShot(200, [&w, startupOpts, positionalImages, loadPluginPath]() {
        int imageCounter = 1;
        for (const QString& imgPath : positionalImages) {
            w.loadImageDirectly(imgPath, QString("Image%1").arg(imageCounter++));
        }
        if (!loadPluginPath.isEmpty()) {
            w.loadAndShowPlugin(loadPluginPath);
        }
        w.applyStartupOptions(startupOpts);
    });

    return app.exec();
}
