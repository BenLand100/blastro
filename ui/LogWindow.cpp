#include "LogWindow.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QScrollBar>
#include <QMetaObject>

namespace blastro {

LogWindow* LogWindow::s_instance = nullptr;
QMutex LogWindow::s_mutex;

LogWindow::LogWindow(QWidget* parent)
    : QWidget(parent) {
    
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    
    // Style the text edit with a dark developer console aesthetic
    m_textEdit->setStyleSheet(
        "QPlainTextEdit {"
        "   background-color: #151515;"
        "   color: #e0e0e0;"
        "   font-family: 'Courier New', Monaco, monospace;"
        "   font-size: 12px;"
        "   border: none;"
        "}"
    );
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_textEdit);
    setLayout(layout);

    registerInstance(this);
}

LogWindow::~LogWindow() {
    unregisterInstance(this);
}

void LogWindow::registerInstance(LogWindow* instance) {
    QMutexLocker locker(&s_mutex);
    s_instance = instance;
}

void LogWindow::unregisterInstance(LogWindow* instance) {
    QMutexLocker locker(&s_mutex);
    if (s_instance == instance) {
        s_instance = nullptr;
    }
}

LogWindow* LogWindow::instance() {
    QMutexLocker locker(&s_mutex);
    return s_instance;
}

void LogWindow::appendMessage(QtMsgType type, const QString& msg) {
    QMutexLocker locker(&s_mutex);
    if (s_instance) {
        // Use QueuedConnection to guarantee thread safety when called from background threads
        QMetaObject::invokeMethod(s_instance, "handleMessage",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, static_cast<int>(type)),
                                  Q_ARG(QString, msg));
    }
}

void LogWindow::handleMessage(int type, const QString& msg) {
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString prefix;
    QString color;

    switch (static_cast<QtMsgType>(type)) {
        case QtDebugMsg:
            prefix = "[DEBUG]";
            color = "#a0a0a0"; // Gray
            break;
        case QtInfoMsg:
            prefix = "[INFO]";
            color = "#00bcd4"; // Cyan
            break;
        case QtWarningMsg:
            prefix = "[WARNING]";
            color = "#ffb300"; // Amber/Yellow
            break;
        case QtCriticalMsg:
            prefix = "[CRITICAL]";
            color = "#f44336"; // Red
            break;
        case QtFatalMsg:
            prefix = "[FATAL]";
            color = "#d32f2f"; // Dark Red
            break;
    }

    // Check if scrollbar is at the bottom before appending
    QScrollBar* bar = m_textEdit->verticalScrollBar();
    bool autoScroll = (bar->value() == bar->maximum());

    // Format message with HTML for coloring
    QString formattedMsg = QString("<span style=\"color:#666666;\">%1</span> "
                                   "<span style=\"color:%2; font-weight:bold;\">%3</span> "
                                   "<span style=\"color:#e0e0e0;\">%4</span>")
                               .arg(timeStr)
                               .arg(color)
                               .arg(prefix)
                               .arg(msg.toHtmlEscaped());

    m_textEdit->appendHtml(formattedMsg);

    if (autoScroll) {
        bar->setValue(bar->maximum());
    }
}

} // namespace blastro
