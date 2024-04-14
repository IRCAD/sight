/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <sight/ui/qml/config.hpp>

#include <ui/__/dialog/logger_base.hpp>

#include <QObject>
#include <QUrl>

#include <vector>

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   logger allowing the choice of an element among severals (_selections)
 */
class SIGHT_UI_QML_CLASS_API logger : public QObject,
                                      public ui::dialog::logger_base
{
Q_OBJECT
Q_PROPERTY(QUrl hidden MEMBER m_hidden WRITE emit_hidden NOTIFY hidden_changed)
Q_PROPERTY(QUrl icon MEMBER m_icon WRITE emit_icon NOTIFY icon_changed)
Q_PROPERTY(QString message MEMBER m_message WRITE emit_message NOTIFY message_changed)
Q_PROPERTY(QUrl shown MEMBER m_shown WRITE emit_shown NOTIFY shown_changed)

public:

    SIGHT_DECLARE_CLASS(logger, ui::dialog::logger_base, ui::factory::make<logger>);

    SIGHT_UI_QML_API logger()           = default;
    SIGHT_UI_QML_API ~logger() override = default;

    /**
     * @brief Set the dialog title.
     * @param[in] _title Dialog title
     */
    SIGHT_UI_QML_API void set_title(const std::string& _title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] _message Dialog message
     */
    SIGHT_UI_QML_API void set_message(const std::string& _message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] _logger Dialog logger
     */
    SIGHT_UI_QML_API void set_logger(const core::log::logger::sptr& _logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    SIGHT_UI_QML_API bool show() override;

Q_SIGNALS:

    /// notify the qml of property change
    void hidden_changed();
    void icon_changed();
    void message_changed();
    void shown_changed();

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void result_dialog(bool _is_ok);

private:

    /// Dialog title
    QString m_title;

    /// Dialog message
    QString m_message;

    /// Logger
    core::log::logger::sptr m_logger;

    /// get pushed button and clicked one
    bool m_is_ok {false};

    /// Resume the biggest error get with an icon
    QUrl m_icon;

    /// Icons path of the logger
    QUrl m_hidden;
    QUrl m_shown;

    /// Setter to QProperty and emit signal
    SIGHT_UI_QML_API void emit_hidden(const QUrl& /*hidden*/);
    SIGHT_UI_QML_API void emit_icon(const QUrl& /*path*/);
    SIGHT_UI_QML_API void emit_message(const QString& /*message*/);
    SIGHT_UI_QML_API void emit_shown(const QUrl& /*shown*/);
};

} // namespace sight::ui::qml::dialog
