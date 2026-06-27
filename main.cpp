#include "ui/MainWindow.h"
#include "ui/LogWindow.h"
#include "core/PCLBridge.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>
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

    if (argc > 3 && (std::strcmp(argv[1], "--test-process") == 0 || std::strcmp(argv[1], "--run-plugin") == 0)) {
        QApplication app(argc, argv);
        QString pluginPath = argv[2];
        QString imagePath = argv[3];
        qDebug() << "Test process execution: plugin =" << pluginPath << "image =" << imagePath;
        
        blastro::MainWindow w;
        w.show();
        
        QTimer::singleShot(100, [&w, pluginPath, imagePath]() {
            w.testProcessOnImage(pluginPath, imagePath);
        });
        
        return app.exec();
    }

    if (argc > 2 && (std::strcmp(argv[1], "--test-register") == 0)) {
        QApplication app(argc, argv);
        QString cubePath = argv[2];
        int refIdx = (argc > 3) ? std::atoi(argv[3]) : 0;
        QString method = (argc > 4) ? argv[4] : "centroid";
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

    if (argc > 2 && (std::strcmp(argv[1], "--test-load") == 0 || std::strcmp(argv[1], "--load-plugin") == 0)) {
        QApplication app(argc, argv);
        QString path = argv[2];
        qDebug() << "Test loading PixInsight module from:" << path << "in GUI mode";
        
        blastro::MainWindow w;
        w.show();
        
        QTimer::singleShot(100, [&w, path]() {
            w.loadAndShowPlugin(path);
        });
        
        return app.exec();
    }

    QApplication app(argc, argv);
    
    // Parse flexible GUI preload arguments and positional image files
    QString loadPluginPath;
    QStringList positionalImages;
    
    QStringList args = QCoreApplication::arguments();
    for (int i = 1; i < args.size(); ++i) {
        QString arg = args.at(i);
        if (arg.startsWith("--")) {
            if (arg == "--load-plugin" || arg == "--test-load" || arg == "--load-image" || arg == "--test-process" || arg == "--run-plugin") {
                if (arg == "--load-plugin" || arg == "--test-load") {
                    if (i + 1 < args.size()) {
                        loadPluginPath = args.at(i + 1);
                    }
                } else if (arg == "--load-image") {
                    if (i + 1 < args.size()) {
                        positionalImages.append(args.at(i + 1));
                    }
                }
                ++i; // Skip the option's value
            }
        } else {
            // Treat as a positional image file
            positionalImages.append(arg);
        }
    }
    
    blastro::MainWindow w;
    w.show();
    
    if (!positionalImages.isEmpty() || !loadPluginPath.isEmpty()) {
        QTimer::singleShot(100, [&w, positionalImages, loadPluginPath]() {
            int imageCounter = 1;
            for (const QString& imgPath : positionalImages) {
                w.loadImageDirectly(imgPath, QString("Image%1").arg(imageCounter++));
            }
            if (!loadPluginPath.isEmpty()) {
                w.loadAndShowPlugin(loadPluginPath);
            }
        });
    }
    
    return app.exec();
}
