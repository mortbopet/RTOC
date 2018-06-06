#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QTimer>

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
    void setDotPrinterState(bool state) {
        if (state) {
            m_acqWaitTimer.start();
        } else {
            m_acqWaitTimer.stop();
        }
    }

private slots:
    void writeDot() { writeToLog("."); }

private:
    QPlainTextEdit* m_log;

    QTimer m_acqWaitTimer;  // timer for printing dots in the log, when awaiting Acquisitor answer
};

#endif  // LOGGER_H
