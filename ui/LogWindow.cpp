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

#include "LogWindow.h"
#include "core/Logger.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QScrollBar>
#include <QMetaObject>
#include <QCloseEvent>

namespace blastro {

static QString parseAnsiToHtml(const QString& input) {
    QString output;
    int i = 0;
    int len = input.length();
    bool bold = false;
    QString currentColor = "";
    int openSpans = 0;

    auto closeSpans = [&]() {
        while (openSpans > 0) {
            output.append("</span>");
            openSpans--;
        }
        bold = false;
        currentColor = "";
    };

    while (i < len) {
        if (input[i] == QChar(27) || input[i] == QChar(0x1B)) {
            if (i + 1 < len && input[i + 1] == '[') {
                int j = i + 2;
                while (j < len && (input[j].isDigit() || input[j] == ';')) {
                    j++;
                }
                if (j < len && input[j] == 'm') {
                    // Extract code parameters
                    QString seq = input.mid(i + 2, j - (i + 2));
                    QStringList parts = seq.split(';');
                    
                    bool reset = false;
                    bool newBold = bold;
                    QString newColor = currentColor;
                    bool hasColorOrBoldChange = false;

                    for (const QString& part : parts) {
                        if (part.isEmpty()) {
                            reset = true;
                            continue;
                        }
                        bool ok = false;
                        int code = part.toInt(&ok);
                        if (!ok) continue;

                        if (code == 0) {
                            reset = true;
                        } else if (code == 1) {
                            newBold = true;
                            hasColorOrBoldChange = true;
                        } else if (code == 22) { // normal intensity
                            newBold = false;
                            hasColorOrBoldChange = true;
                        } else if (code >= 30 && code <= 37) {
                            // Standard foreground colors
                            static const QString colors[] = {
                                "#7f7f7f", // 30 Black
                                "#f44336", // 31 Red
                                "#4caf50", // 32 Green
                                "#ffeb3b", // 33 Yellow
                                "#2196f3", // 34 Blue
                                "#e040fb", // 35 Magenta
                                "#00bcd4", // 36 Cyan
                                "#ffffff"  // 37 White
                            };
                            newColor = colors[code - 30];
                            hasColorOrBoldChange = true;
                        } else if (code == 39) {
                            newColor = ""; // Default foreground
                            hasColorOrBoldChange = true;
                        } else if (code >= 90 && code <= 97) {
                            // Bright foreground colors
                            static const QString colors[] = {
                                "#b0bec5", // 90 Bright Black (Gray)
                                "#ff8a80", // 91 Bright Red
                                "#b9f6ca", // 92 Bright Green
                                "#ffe57f", // 93 Bright Yellow
                                "#80d8ff", // 94 Bright Blue
                                "#ff80f0", // 95 Bright Magenta
                                "#84ffff", // 96 Bright Cyan
                                "#ffffff"  // 97 Bright White
                            };
                            newColor = colors[code - 90];
                            hasColorOrBoldChange = true;
                        }
                    }

                    if (reset) {
                        closeSpans();
                    } else if (hasColorOrBoldChange) {
                        closeSpans();
                        bold = newBold;
                        currentColor = newColor;

                        QString style;
                        if (!currentColor.isEmpty()) {
                            style += QString("color:%1;").arg(currentColor);
                        }
                        if (bold) {
                            style += "font-weight:bold;";
                        }
                        if (!style.isEmpty()) {
                            output.append(QString("<span style=\"%1\">").arg(style));
                            openSpans++;
                        }
                    }

                    i = j + 1; // Skip past 'm'
                    continue;
                }
            }
        }
        
        output.append(input[i]);
        i++;
    }

    closeSpans();
    return output;
}

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

void LogWindow::closeEvent(QCloseEvent* event) {
    // Ignore close so the QMdiSubWindow hides itself without hiding
    // this widget — log history is preserved for when it is reopened.
    event->ignore();
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
                               .arg(parseAnsiToHtml(msg.toHtmlEscaped()));

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

    bool isUpdate = message.startsWith('\r');
    QString cleanMsg = message;
    if (isUpdate) {
        cleanMsg.remove(0, 1);
    }

    // Check if scrollbar is at the bottom before appending
    QScrollBar* bar = m_textEdit->verticalScrollBar();
    bool autoScroll = (bar->value() == bar->maximum());

    // Format message with HTML for coloring and layout
    QString formattedMsg;
    QString parsedMessage = parseAnsiToHtml(cleanMsg.toHtmlEscaped());
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
                           .arg(parsedMessage);
    } else {
        formattedMsg = QString("<span style=\"color:#666666;\">[%1]</span> "
                               "<span style=\"color:%2; font-weight:bold;\">[%3]</span> "
                               "<span style=\"color:%4; font-weight:%5;\">%6</span>")
                           .arg(timeStr)
                           .arg(channelColor)
                           .arg(channel)
                           .arg(msgColor)
                           .arg(msgWeight)
                           .arg(parsedMessage);
    }

    if (isUpdate && m_textEdit->document()->blockCount() > 0) {
        QTextCursor cursor(m_textEdit->document());
        cursor.movePosition(QTextCursor::End);
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertHtml(formattedMsg);
    } else {
        m_textEdit->appendHtml(formattedMsg);
    }

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
