/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwGuiQml/config.hpp"

#include <fwGui/dialog/ILoggerDialog.hpp>

#include <QObject>
#include <QUrl>

#include <vector>

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   LoggerDialog allowing the choice of an element among severals (_selections)
 */
class FWGUIQML_CLASS_API LoggerDialog : public QObject,
                                        public ::fwGui::dialog::ILoggerDialog
{
Q_OBJECT
Q_PROPERTY(QUrl critical MEMBER m_critical WRITE emitCritical NOTIFY criticalChanged)
Q_PROPERTY(QUrl hidden MEMBER m_hidden WRITE emitHidden NOTIFY hiddenChanged)
Q_PROPERTY(QUrl icon MEMBER m_icon WRITE emitIcon NOTIFY iconChanged)
Q_PROPERTY(QUrl information MEMBER m_information WRITE emitInformation NOTIFY informationChanged)
Q_PROPERTY(QString message MEMBER m_message WRITE emitMessage NOTIFY messageChanged)
Q_PROPERTY(QUrl shown MEMBER m_shown WRITE emitShown NOTIFY shownChanged)
Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
Q_PROPERTY(bool visible MEMBER m_visible)
Q_PROPERTY(QUrl warning MEMBER m_warning WRITE emitWarning NOTIFY warningChanged)

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LoggerDialog)(::fwGui::dialog::ILoggerDialog),
                                            (()),
                                            ::fwGui::factory::New< LoggerDialog > );

    /// Constructor
    FWGUIQML_API LoggerDialog(::fwGui::GuiBaseObject::Key key);

    /// Destructor
    FWGUIQML_API virtual ~LoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUIQML_API virtual void setTitle(const std::string& title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUIQML_API virtual void setMessage(const std::string& message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUIQML_API virtual void setLogger(const ::fwLog::Logger::sptr& logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUIQML_API virtual bool show() override;

Q_SIGNALS:
    /// notify the qml of property change
    void criticalChanged();
    void hiddenChanged();
    void iconChanged();
    void informationChanged();
    void messageChanged();
    void shownChanged();
    void titleChanged();
    void warningChanged();

protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(bool isOk);

private:
    /// Dialog title
    QString m_title;

    /// Dialog message
    QString m_message;

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// get pushed button and clicked one
    bool m_isOk;
    bool m_isClicked;
    /// visible state of the logger
    bool m_visible;

    /// Resume the biggest error get with an icon
    QUrl m_icon;

    /// Icons path of the logger
    QUrl m_critical;
    QUrl m_hidden;
    QUrl m_information;
    QUrl m_shown;
    QUrl m_warning;

    /// Setter to QProperty and emit signal
    FWGUIQML_API void emitCritical(const QUrl&);
    FWGUIQML_API void emitHidden(const QUrl&);
    FWGUIQML_API void emitIcon(const QUrl&);
    FWGUIQML_API void emitInformation(const QUrl&);
    FWGUIQML_API void emitMessage(const QString&);
    FWGUIQML_API void emitShown(const QUrl&);
    FWGUIQML_API void emitWarning(const QUrl&);
};

} // namespace dialog
} // namespace fwGuiQml
