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

#include <core/base.hpp>

#include <ui/__/dialog/input_base.hpp>

#include <QObject>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   ui::dialog::input inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.set_input ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class UI_QML_CLASS_API input : public QObject,
                               public ui::dialog::input_base
{
Q_OBJECT
Q_ENUM(EchoMode)
Q_PROPERTY(QString input MEMBER m_input NOTIFY inputChanged)
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
Q_PROPERTY(EchoMode echoMode MEMBER m_echoMode NOTIFY echoModeChanged)

public:

    SIGHT_DECLARE_CLASS(input, ui::dialog::input_base, ui::factory::make<input>);

    UI_QML_API ~input() override = default;

    /// Set the title of the message box
    UI_QML_API void setTitle(const std::string& _title) override;

    /// Set the message
    UI_QML_API void setMessage(const std::string& _msg) override;

    /// Sets the echo mode used to display input field content
    UI_QML_API void setEchoMode(input::EchoMode _echo_mode) override;

    /// Set the input text in the input field
    UI_QML_API void set_input(const std::string& _text) override;

    /// Get the input text in the input field
    UI_QML_API std::pair<std::string, bool> getInput() override;

Q_SIGNALS:

    /// notify the qml of property change
    void inputChanged();
    void messageChanged();
    void titleChanged();
    void echoModeChanged();

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& _msg, bool _is_ok);

private:

    /// Dialog title
    QString m_title;

    /// Dialog box message
    QString m_message;

    /// Text input
    QString m_input;

    /// Echo mode for the TextField
    EchoMode m_echoMode {EchoMode::DEFAULT};
};

} // namespace sight::ui::qml::dialog
