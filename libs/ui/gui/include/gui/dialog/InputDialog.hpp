/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "gui/config.hpp"
#include "gui/dialog/IInputDialog.hpp"
#include "gui/GuiBaseObject.hpp"

namespace sight::gui
{
namespace dialog
{
/**
 * @brief   Defines the generic input dialog for IHM.
 * Use the Delegate design pattern
 *
 * Example of use:
 * @code{.xml}
   gui::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class GUI_CLASS_API InputDialog : public gui::dialog::IInputDialog
{

public:

    fwCoreClassMacro(InputDialog, gui::dialog::IInputDialog, new InputDialog)

    /**
     * Opens a default input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     * @return Returns text in the input field
     */
    GUI_API static std::string showInputDialog(const std::string& title, const std::string& message,
                                               const std::string& text = "");

    /** @brief Constructor. Create the implementation of the specific input dialog box.
     *
     * Example of use:
     * @code{.xml}
       gui::dialog::InputDialog inputDlg;
       inputDlg.setTitle("Identification dialog");
       inputDlg.setMessage("Enter Pin Code: ");
       inputDlg.setInput ("<Enter your code here>");
       std::string inputText = inputDlg.getInput();
       @endcode
     */
    GUI_API InputDialog();

    /**
     * Constructor. Creates a input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     */
    GUI_API InputDialog(const std::string& title, const std::string& message, const std::string& text = "");

    /// Destructor. Do nothing
    GUI_API virtual ~InputDialog();

    /// Set the title of the input dialog
    GUI_API virtual void setTitle(const std::string& title) override;

    /// Set the message
    GUI_API virtual void setMessage(const std::string& msg) override;

    /// Set the input text in the input field
    GUI_API virtual void setInput(const std::string& text) override;

    /// Get the input text in the input field
    GUI_API virtual std::string getInput() override;

protected:

    /// Implementation of an input dialog in a specific IHM (wx/Qt)
    gui::dialog::IInputDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace sight::gui
