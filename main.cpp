#include "ui/MainWindow.h"
#include "core/PCLBridge.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <cstring>

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
