/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "guiQt/config.hpp"

#include <core/base.hpp>

#include <gui/dialog/IInputDialog.hpp>

#include <string>

namespace sight::guiQt
{
namespace dialog
{
/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   gui::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class GUIQT_CLASS_API InputDialog : public gui::dialog::IInputDialog
{

public:

    fwCoreClassMacro(InputDialog, gui::dialog::IInputDialog, gui::factory::New< InputDialog >)

    GUIQT_API InputDialog(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~InputDialog();

    /// Set the title of the message box
    GUIQT_API virtual void setTitle( const std::string& title ) override;

    /// Set the message
    GUIQT_API virtual void setMessage( const std::string& msg ) override;

    /// Set the input text in the input field
    GUIQT_API virtual void setInput(const std::string& text) override;

    /// Get the input text in the input field
    GUIQT_API virtual std::string getInput() override;

protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// Text inputed
    std::string m_input;

};
} // namespace dialog
} // namespace sight::guiQt
