#include "LogWindow.h"
#include "core/Logger.h"
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

    Logger::registerCallback([](Logger::Level level, const QString& channel, const QString& message) {
        QString levelStr;
        switch (level) {
            case Logger::Level::Info:    levelStr = "info"; break;
            case Logger::Level::Success: levelStr = "success"; break;
            case Logger::Level::Warning: levelStr = "warning"; break;
            case Logger::Level::Error:   levelStr = "error"; break;
            case Logger::Level::Header:  levelStr = "header"; break;
        }
        LogWindow::appendRichMessage(channel, message, levelStr);
    });
}

LogWindow::~LogWindow() {
    Logger::registerCallback(nullptr);
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

void LogWindow::appendRichMessage(const QString& channel, const QString& message, const QString& levelStr) {
    QMutexLocker locker(&s_mutex);
    if (s_instance) {
        QMetaObject::invokeMethod(s_instance, "handleRichMessage",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, channel),
                                  Q_ARG(QString, message),
                                  Q_ARG(QString, levelStr));
    }
}

void LogWindow::handleRichMessage(const QString& channel, const QString& message, const QString& levelStr) {
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString channelColor = "#00bcd4"; // cyan
    QString msgColor = "#e0e0e0";     // light gray
    QString msgWeight = "normal";
    
    if (levelStr == "success") {
        channelColor = "#4caf50";    // soft green
        msgColor = "#a3e2a5";        // light green
        msgWeight = "bold";
    } else if (levelStr == "warning") {
        channelColor = "#ffb300";    // soft yellow
        msgColor = "#ffe082";        // light yellow
    } else if (levelStr == "error") {
        channelColor = "#f44336";    // soft red
        msgColor = "#ffcdd2";        // light red
        msgWeight = "bold";
    } else if (levelStr == "header") {
        channelColor = "#e040fb";    // soft magenta
        msgColor = "#f3e5f5";        // light magenta
        msgWeight = "bold";
    }

    // Check if scrollbar is at the bottom before appending
    QScrollBar* bar = m_textEdit->verticalScrollBar();
    bool autoScroll = (bar->value() == bar->maximum());

    // Format message with HTML for coloring and layout
    QString formattedMsg;
    if (levelStr == "header") {
        formattedMsg = QString("<div style=\"margin-top: 8px; margin-bottom: 4px;\">"
                               "<span style=\"color:#666666;\">[%1]</span> "
                               "<span style=\"color:%2; font-weight:bold;\">[%3]</span> "
                               "<span style=\"color:%4; font-weight:%5;\">%6</span>"
                               "</div>")
                           .arg(timeStr)
                           .arg(channelColor)
                           .arg(channel)
                           .arg(msgColor)
                           .arg(msgWeight)
                           .arg(message.toHtmlEscaped());
    } else {
        formattedMsg = QString("<span style=\"color:#666666;\">[%1]</span> "
                               "<span style=\"color:%2; font-weight:bold;\">[%3]</span> "
                               "<span style=\"color:%4; font-weight:%5;\">%6</span>")
                           .arg(timeStr)
                           .arg(channelColor)
                           .arg(channel)
                           .arg(msgColor)
                           .arg(msgWeight)
                           .arg(message.toHtmlEscaped());
    }

    m_textEdit->appendHtml(formattedMsg);

    if (autoScroll) {
        bar->setValue(bar->maximum());
    }
}

void LogWindow::appendRawText(const QString& text, const QString& colorHtml) {
    QScrollBar* bar = m_textEdit->verticalScrollBar();
    bool autoScroll = (bar->value() == bar->maximum());

    QString formattedMsg = QString("<pre style=\"color:%1; margin:0; font-family:'Courier New', Monaco, monospace; font-size:12px; line-height:1.2;\">%2</pre>")
                               .arg(colorHtml)
                               .arg(text.toHtmlEscaped());

    m_textEdit->appendHtml(formattedMsg);

    if (autoScroll) {
        bar->setValue(bar->maximum());
    }
}

} // namespace blastro
