/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/config.hpp"
#include "ui/qml/dialog/StandardButton.hpp"

#include <ui/__/dialog/message_base.hpp>

#include <QObject>
#include <QUrl>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief Defines the generic message box for IHM.
 */
class UI_QML_CLASS_API message : public QObject,
                                 public ui::dialog::message_base
{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
Q_PROPERTY(QUrl icon MEMBER m_iconImage NOTIFY iconChanged)

public:

    SIGHT_DECLARE_CLASS(message, ui::dialog::message_base, ui::factory::make<message>);

    UI_QML_API message();

    UI_QML_API ~message() override;

    /// Set the title of the message box
    UI_QML_API void setTitle(const std::string& title) override;

    /// Set the message
    UI_QML_API void setMessage(const std::string& msg) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    UI_QML_API void setIcon(message::Icons icon) override;

    /// Add a button (OK, YES_NO, CANCEL)
    UI_QML_API void addButton(message::Buttons button) override;

    /// Add a custom button to this dialog
    UI_QML_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    /// Set the default buttons
    UI_QML_API void setDefaultButton(message::Buttons button) override;

    /// Show the message box and return the clicked button.
    UI_QML_API Buttons show() override;

Q_SIGNALS:

    /// notify the qml of property change
    void buttonsChanged();
    void messageChanged();
    void iconChanged();

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(int clicked);

private:

    /// Dialog title
    QString m_title;

    /// Dialog box message
    QString m_message;

    message::Buttons m_buttons {message::NOBUTTON};

    /// Icon
    message::Icons m_icon {message::NONE};

    /// Resume the biggest error get with an icon
    QUrl m_iconImage {""};

    /// boolean to check if button was pressed
    message::Buttons m_clicked {message::NOBUTTON};

    /// Setter to QProperty and emit signal
    UI_QML_API void emitButtons(StandardButton* /*standardButton*/);
    UI_QML_API void emitIcon(const QUrl& iconPath);
};

} // namespace sight::ui::qml::dialog
