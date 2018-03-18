#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPlainTextEdit>

/**
 * @brief The Logger class
 * @abstract Object used for parsing status messages between the async Acquisitor process and the UI
 */
class Logger : public QObject {
    Q_OBJECT
public:
    Logger(QObject* parent = nullptr);

    void setLog(QPlainTextEdit* log) { m_log = log; }
public slots:
    void writeLineToLog(QString msg);
    void writeToLog(QString msg);

private:
    QPlainTextEdit* m_log;
};

#endif  // LOGGER_H
