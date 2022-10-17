/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/config.hpp"

#include <core/base.hpp>

#include <ui/base/dialog/IInputDialog.hpp>

#include <string>

namespace sight::ui::qt::dialog
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
class UI_QT_CLASS_API InputDialog : public ui::base::dialog::IInputDialog
{
public:

    SIGHT_DECLARE_CLASS(InputDialog, ui::base::dialog::IInputDialog, ui::base::factory::New<InputDialog>);

    UI_QT_API InputDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API ~InputDialog() override;

    /// Set the title of the message box
    UI_QT_API void setTitle(const std::string& title) override;

    /// Set the message
    UI_QT_API void setMessage(const std::string& msg) override;

    /// Sets the echo mode used to display input field content
    UI_QT_API void setEchoMode(base::dialog::IInputDialog::EchoMode echoMode) override;

    /// Set the input text in the input field
    UI_QT_API void setInput(const std::string& text) override;

    /// Get the input text in the input field
    UI_QT_API std::string getInput() override;

protected:

    /// Dialog title
    std::string m_title;

    /// Dialog box message
    std::string m_message;

    /// Text input
    std::string m_input;

    /// Echo mode
    base::dialog::IInputDialog::EchoMode m_echoMode {base::dialog::IInputDialog::EchoMode::DEFAULT};
};

} // namespace sight::ui::qt::dialog
