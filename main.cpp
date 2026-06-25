#include "ui/MainWindow.h"
#include "core/PCLBridge.h"
#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc > 2 && std::strcmp(argv[1], "--test-load") == 0) {
        QCoreApplication app(argc, argv);
        QString path = argv[2];
        qDebug() << "Test loading PixInsight module from:" << path;
        
        blastro::PCLBridge bridge;
        if (bridge.loadModule(path)) {
            qDebug() << "SUCCESS: Module loaded and initialized successfully!";
            
            auto processes = bridge.registeredProcesses();
            qDebug() << "Registered processes:";
            for (const auto& proc : processes) {
                qDebug() << "  -" << proc;
            }
            return 0;
        } else {
            qWarning() << "FAILURE: Failed to load module.";
            return 1;
        }
    }

    QApplication app(argc, argv);
    
    blastro::MainWindow w;
    w.show();
    
    return app.exec();
}
