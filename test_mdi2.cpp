#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow win;
    QMdiArea *mdi = new QMdiArea(&win);
    win.setCentralWidget(mdi);

    QWidget *host = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(host);
    lay->setSizeConstraint(QLayout::SetFixedSize);
    QPushButton *btn = new QPushButton("Toggle");
    QPushButton *hideBtn = new QPushButton("Hidden");
    hideBtn->setFixedSize(200, 200);
    lay->addWidget(btn);
    lay->addWidget(hideBtn);

    QMdiSubWindow *sub = mdi->addSubWindow(host);
    sub->show();
    win.show();

    QTimer::singleShot(500, [&app, sub, host, hideBtn]() {
        std::cout << "Initial subwindow size: " << sub->width() << "x" << sub->height() << std::endl;
        std::cout << "Initial host size: " << host->width() << "x" << host->height() << std::endl;
        hideBtn->hide();
    });

    QTimer::singleShot(1000, [&app, sub, host]() {
        std::cout << "After hide subwindow size: " << sub->width() << "x" << sub->height() << std::endl;
        std::cout << "After hide host size: " << host->width() << "x" << host->height() << std::endl;
        
        // try adjustSize
        sub->adjustSize();
    });
    
    QTimer::singleShot(1500, [&app, sub, host]() {
        std::cout << "After adjustSize subwindow size: " << sub->width() << "x" << sub->height() << std::endl;
        std::cout << "After adjustSize host size: " << host->width() << "x" << host->height() << std::endl;
        
        // try resize(sizeHint)
        sub->resize(sub->sizeHint());
    });

    QTimer::singleShot(2000, [&app, sub, host]() {
        std::cout << "After resize(sizeHint) subwindow size: " << sub->width() << "x" << sub->height() << std::endl;
        std::cout << "After resize(sizeHint) host size: " << host->width() << "x" << host->height() << std::endl;
        app.quit();
    });

    return app.exec();
}
