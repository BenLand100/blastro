#include "Logger.h"
#include <iostream>
#include <QDateTime>
#include <QMetaType>
#include <QMutex>

namespace blastro {

Logger::LogCallback Logger::s_callback = nullptr;

void Logger::registerCallback(LogCallback callback) {
    s_callback = callback;
}

static QMutex s_termMutex;

void Logger::log(Level level, const std::string& channel, const std::string& message) {
    log(level, QString::fromStdString(channel), QString::fromStdString(message));
}

void Logger::log(Level level, const QString& channel, const QString& message) {
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    
    // 1. Format for Terminal Output (stdout/stderr with ANSI Colors)
    std::string ansiColor = "\033[0m"; // default/reset
    std::string ansiLevel = "";
    bool isErrorOrWarning = false;

    switch (level) {
        case Level::Info:
            ansiColor = "\033[1;36m"; // Bold Cyan
            ansiLevel = "INFO";
            break;
        case Level::Success:
            ansiColor = "\033[1;32m"; // Bold Green
            ansiLevel = "SUCCESS";
            break;
        case Level::Warning:
            ansiColor = "\033[1;33m"; // Bold Yellow
            ansiLevel = "WARNING";
            isErrorOrWarning = true;
            break;
        case Level::Error:
            ansiColor = "\033[1;31m"; // Bold Red
            ansiLevel = "ERROR";
            isErrorOrWarning = true;
            break;
        case Level::Header:
            ansiColor = "\033[1;35m"; // Bold Magenta
            ansiLevel = "HEADER";
            break;
    }

    std::string termTime = "\033[90m[" + timeStr.toStdString() + "]\033[0m";
    std::string termChannel = ansiColor + "[" + channel.toStdString() + "]\033[0m";
    std::string termMsg = message.toStdString();

    // Print to stdout or stderr depending on severity (thread-safe serialization)
    {
        QMutexLocker locker(&s_termMutex);
        if (isErrorOrWarning) {
            std::cerr << termTime << " " << termChannel << " " << termMsg << std::endl;
        } else {
            std::cout << termTime << " " << termChannel << " " << termMsg << std::endl;
        }
    }

    // 2. Forward to the registered callback (e.g., UI Process Console)
    if (s_callback) {
        s_callback(level, channel, message);
    }
}

void Logger::log(Level level, const char* channel, const char* message) {
    log(level, QString::fromUtf8(channel), QString::fromUtf8(message));
}

// Convenience methods using QString
void Logger::info(const QString& channel, const QString& message) { log(Level::Info, channel, message); }
void Logger::success(const QString& channel, const QString& message) { log(Level::Success, channel, message); }
void Logger::warning(const QString& channel, const QString& message) { log(Level::Warning, channel, message); }
void Logger::error(const QString& channel, const QString& message) { log(Level::Error, channel, message); }
void Logger::header(const QString& channel, const QString& message) { log(Level::Header, channel, message); }

// Convenience methods using std::string
void Logger::info(const std::string& channel, const std::string& message) { log(Level::Info, channel, message); }
void Logger::success(const std::string& channel, const std::string& message) { log(Level::Success, channel, message); }
void Logger::warning(const std::string& channel, const std::string& message) { log(Level::Warning, channel, message); }
void Logger::error(const std::string& channel, const std::string& message) { log(Level::Error, channel, message); }
void Logger::header(const std::string& channel, const std::string& message) { log(Level::Header, channel, message); }

// Convenience methods using const char*
void Logger::info(const char* channel, const char* message) { log(Level::Info, channel, message); }
void Logger::success(const char* channel, const char* message) { log(Level::Success, channel, message); }
void Logger::warning(const char* channel, const char* message) { log(Level::Warning, channel, message); }
void Logger::error(const char* channel, const char* message) { log(Level::Error, channel, message); }
void Logger::header(const char* channel, const char* message) { log(Level::Header, channel, message); }

} // namespace blastro
