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
        // try adjustSize
        sub->adjustSize();
        std::cout << "After adjustSize subwindow size: " << sub->width() << "x" << sub->height() << std::endl;
        std::cout << "After adjustSize host size: " << host->width() << "x" << host->height() << std::endl;
    });

    QTimer::singleShot(1500, [&app, sub, host]() {
        app.quit();
    });

    return app.exec();
}
