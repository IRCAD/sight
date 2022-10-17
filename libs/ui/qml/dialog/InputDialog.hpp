/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include <ui/base/dialog/IInputDialog.hpp>

#include <QObject>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   ui::base::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class UI_QML_CLASS_API InputDialog : public QObject,
                                     public ui::base::dialog::IInputDialog
{
Q_OBJECT
Q_ENUM(EchoMode)
Q_PROPERTY(QString input MEMBER m_input NOTIFY inputChanged)
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
Q_PROPERTY(EchoMode echoMode MEMBER m_echoMode NOTIFY echoModeChanged)

public:

    SIGHT_DECLARE_CLASS(InputDialog, ui::base::dialog::IInputDialog, ui::base::factory::New<InputDialog>);

    UI_QML_API InputDialog(ui::base::GuiBaseObject::Key key);

    UI_QML_API ~InputDialog() override;

    /// Set the title of the message box
    UI_QML_API void setTitle(const std::string& title) override;

    /// Set the message
    UI_QML_API void setMessage(const std::string& msg) override;

    /// Sets the echo mode used to display input field content
    UI_QML_API void setEchoMode(base::dialog::IInputDialog::EchoMode echoMode) override;

    /// Set the input text in the input field
    UI_QML_API void setInput(const std::string& text) override;

    /// Get the input text in the input field
    UI_QML_API std::string getInput() override;

Q_SIGNALS:

    /// notify the qml of property change
    void inputChanged();
    void messageChanged();
    void titleChanged();
    void echoModeChanged();

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& msg, bool isOk);

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
