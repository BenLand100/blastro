#pragma once
#include <QString>
#include <string>
#include <functional>

namespace blastro {

class Logger {
public:
    enum class Level {
        Info,
        Success,
        Warning,
        Error,
        Header
    };

    using LogCallback = std::function<void(Level level, const QString& channel, const QString& message)>;

    // Register a callback to route logs to the UI or other systems
    static void registerCallback(LogCallback callback);

    // Low-level log routing
    static void log(Level level, const std::string& channel, const std::string& message);
    static void log(Level level, const QString& channel, const QString& message);
    static void log(Level level, const char* channel, const char* message);

    // Convenience methods using QString
    static void info(const QString& channel, const QString& message);
    static void success(const QString& channel, const QString& message);
    static void warning(const QString& channel, const QString& message);
    static void error(const QString& channel, const QString& message);
    static void header(const QString& channel, const QString& message);

    // Convenience methods using std::string
    static void info(const std::string& channel, const std::string& message);
    static void success(const std::string& channel, const std::string& message);
    static void warning(const std::string& channel, const std::string& message);
    static void error(const std::string& channel, const std::string& message);
    static void header(const std::string& channel, const std::string& message);

    // Convenience methods using const char*
    static void info(const char* channel, const char* message);
    static void success(const char* channel, const char* message);
    static void warning(const char* channel, const char* message);
    static void error(const char* channel, const char* message);
    static void header(const char* channel, const char* message);

private:
    static LogCallback s_callback;
};

} // namespace blastro
