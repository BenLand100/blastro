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

#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QString>
#include <QMutex>

namespace blastro {

class LogWindow : public QWidget {
    Q_OBJECT
public:
    explicit LogWindow(QWidget* parent = nullptr);
    ~LogWindow() override;

    // Registers the log window instance globally/locally for the message handler
    static void registerInstance(LogWindow* instance);
    static void unregisterInstance(LogWindow* instance);
    static LogWindow* instance();

    // Thread-safe method to append messages
    static void appendMessage(QtMsgType type, const QString& msg);
    static void appendRichMessage(const QString& channel, const QString& message, const QString& levelStr);

    // Append raw formatted text (e.g., for welcome screen/ASCII art)
    void appendRawText(const QString& text, const QString& colorHtml = "#e0e0e0");

public slots:
    void handleMessage(int type, const QString& msg);
    void handleRichMessage(const QString& channel, const QString& message, const QString& levelStr);

private:
    QPlainTextEdit* m_textEdit;
    static LogWindow* s_instance;
    static QMutex s_mutex;
};

} // namespace blastro
