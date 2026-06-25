#include "ui/MainWindow.h"
#include "core/PCLBridge.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <cstring>

int main(int argc, char* argv[]) {
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
    
    blastro::MainWindow w;
    w.show();
    
    return app.exec();
}
