#pragma once

#include "PoCDialog/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/IInputDialog.hpp>

#include <QObject>

#include <string>

/**
 * @brief   This class is associated to LoggerDialogs.qml and calls
 * LoggerDialog with the selected options
 */
class POCDIALOG_CLASS_API LoggerDialogs : public QObject
{
Q_OBJECT
Q_PROPERTY(QStringList errorLevel MEMBER m_errorLevel)
Q_PROPERTY(QStringList errorMessage MEMBER m_errorMessage)
Q_PROPERTY(QString title MEMBER m_title)
Q_PROPERTY(QString message MEMBER m_message)

public:

    POCDIALOG_API LoggerDialogs();

    POCDIALOG_API virtual ~LoggerDialogs() override;

protected:
    /// Dialog box message
    QString m_message;
    /// the Logger list of error
    QStringList m_errorLevel;
    QStringList m_errorMessage;
    /// title
    QString m_title;

protected Q_SLOTS:
    void open();
};
