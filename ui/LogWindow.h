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

public slots:
    void handleMessage(int type, const QString& msg);

private:
    QPlainTextEdit* m_textEdit;
    static LogWindow* s_instance;
    static QMutex s_mutex;
};

} // namespace blastro
