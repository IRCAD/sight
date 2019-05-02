#pragma once

#include "PoCDialog/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/IMessageDialog.hpp>

#include <QObject>

#include <string>

/**
 * @brief   This class is associated to InputDialogs.qml and calls
 * InputDialog with the selected options
 */
class POCDIALOG_CLASS_API MessageDialogs : public QObject
{
Q_OBJECT
Q_PROPERTY(int icon MEMBER m_icon)
Q_PROPERTY(QString message MEMBER m_message)
Q_PROPERTY(QString title MEMBER m_title)
Q_PROPERTY(int buttons MEMBER m_buttons)
Q_PROPERTY(QString result MEMBER m_result NOTIFY onResultChanged)

public:

    POCDIALOG_API MessageDialogs();

    POCDIALOG_API virtual ~MessageDialogs() override;

Q_SIGNALS:
    void onResultChanged();

protected:
    /// Dialog title
    QString m_title;
    /// Dialog box message
    QString m_message;
    /// Text inputed
    QString m_input;
    /// icon of the message dialog
    int m_icon;
    /// the buttons to add to the dialog
    int m_buttons;
    QString m_result;

protected Q_SLOTS:
    void open();
};
