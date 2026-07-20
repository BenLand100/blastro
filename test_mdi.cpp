#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QVBoxLayout>

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
    lay->addWidget(btn);
    lay->addWidget(hideBtn);

    QObject::connect(btn, &QPushButton::clicked, [=]() {
        hideBtn->setVisible(!hideBtn->isVisible());
    });

    QMdiSubWindow *sub = mdi->addSubWindow(host);
    sub->show();
    win.show();

    return app.exec();
}
