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

    QCommandLineOption noRestoreOption(QStringList() << "n" << "no-restore", "Do not restore the last session on startup.");
    parser.addOption(noRestoreOption);

    QCommandLineOption projectOption(QStringList() << "p" << "project", "Open a specific project directory on startup.", "project_path");
    parser.addOption(projectOption);

    QCommandLineOption sessionOption(QStringList() << "s" << "session", "Open a specific session file on startup.", "session_path");
    parser.addOption(sessionOption);

    QCommandLineOption imageOption(QStringList() << "i" << "image", "Load the specified image file on startup.", "image_path");
    parser.addOption(imageOption);

    QCommandLineOption exitOption(QStringList() << "e" << "exit", "Exit the application automatically after completing any CLI actions.");
    parser.addOption(exitOption);
    
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Start the application in fullscreen mode.");
    parser.addOption(fullscreenOption);

    QCommandLineOption runPclOption("run-pcl", "Load a PCL module and execute its process on the active image.", "plugin_path");
    parser.addOption(runPclOption);
    
    QCommandLineOption runAlgoOption("run-algo", "Run a built-in algorithm by name (e.g., Register, Stack).", "algo_name");
    parser.addOption(runAlgoOption);
    
    QCommandLineOption optsOption("opts", "Options for process/algorithm (e.g., key=val,key2=val2).", "key_value_pairs");
    parser.addOption(optsOption);

    QCommandLineOption testRegisterOption("test-register", "Run a registration test on a cube.", "cube_path");
    parser.addOption(testRegisterOption);

    QCommandLineOption refIdxOption("ref-idx", "Reference frame index for registration test (default: 0).", "index");
    parser.addOption(refIdxOption);

    QCommandLineOption methodOption("method", "Alignment method for registration test (default: centroid).", "method");
    parser.addOption(methodOption);

    parser.addPositionalArgument("images", "Image files to load on startup.", "[images...]");

    parser.process(app);

    blastro::StartupOptions startupOpts;
    startupOpts.noRestore = parser.isSet(noRestoreOption);
    startupOpts.exitAfterLoad = parser.isSet(exitOption);
    startupOpts.fullscreen = parser.isSet(fullscreenOption);
    
    if (parser.isSet(projectOption)) {
        startupOpts.projectPath = parser.value(projectOption);
    }
    if (parser.isSet(sessionOption)) {
        startupOpts.sessionPath = parser.value(sessionOption);
    }
    if (parser.isSet(runPclOption)) {
        startupOpts.runPclPath = parser.value(runPclOption);
    }
    if (parser.isSet(runAlgoOption)) {
        startupOpts.runAlgoName = parser.value(runAlgoOption);
    }
    if (parser.isSet(optsOption)) {
        startupOpts.algoOptsStr = parser.value(optsOption);
    }
    if (parser.isSet(testRegisterOption)) {
        startupOpts.testRegisterCube = parser.value(testRegisterOption);
    }
    if (parser.isSet(refIdxOption)) {
        startupOpts.refIdx = parser.value(refIdxOption).toInt();
    }
    if (parser.isSet(methodOption)) {
        startupOpts.method = parser.value(methodOption);
    }

    QStringList imagesToLoad = parser.positionalArguments();
    if (parser.isSet(imageOption)) {
        for (const QString& img : parser.values(imageOption)) {
            imagesToLoad.append(img);
        }
    }
    startupOpts.imagesToLoad = imagesToLoad;

    blastro::MainWindow w;
    w.show();

    QTimer::singleShot(100, [&w, startupOpts]() {
        w.applyStartupOptions(startupOpts);
    });

    return app.exec();
}
