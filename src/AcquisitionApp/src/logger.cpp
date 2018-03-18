#include "logger.h"

Logger::Logger() {}

void Logger::writeLineToLog(QString msg) {
    if (msg.at(msg.length() - 1) != '\n') {
        msg.append('\n');
    }
    m_log->appendPlainText(msg);
}
void Logger::writeToLog(QString msg) {
    m_log->appendPlainText(msg);
}
