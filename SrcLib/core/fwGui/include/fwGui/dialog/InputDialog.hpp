/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGUI_DIALOG_INPUTDIALOG_HPP__
#define __FWGUI_DIALOG_INPUTDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/dialog/IInputDialog.hpp"
#include "fwGui/GuiBaseObject.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic input dialog for IHM.
 * Use the Delegate design pattern
 *
 * Example of use:
 * @code{.xml}
   ::fwGui::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class FWGUI_CLASS_API InputDialog : public ::fwGui::dialog::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::dialog::IInputDialog), (()), new InputDialog )

    /**
     * Opens a default input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     * @return Returns text in the input field
     */
    FWGUI_API static std::string showInputDialog(const std::string& title, const std::string& message,
                                                 const std::string& text = "");

    /** @brief Constructor. Create the implementation of the specific input dialog box.
     *
     * Example of use:
     * @code{.xml}
       ::fwGui::dialog::InputDialog inputDlg;
       inputDlg.setTitle("Identification dialog");
       inputDlg.setMessage("Enter Pin Code: ");
       inputDlg.setInput ("<Enter your code here>");
       std::string inputText = inputDlg.getInput();
       @endcode
     */
    FWGUI_API InputDialog();

    /**
     * Constructor. Creates a input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     */
    FWGUI_API InputDialog(const std::string& title, const std::string& message, const std::string& text = "");

    /// Destructor. Do nothing
    FWGUI_API virtual ~InputDialog();

    /// Set the title of the input dialog
    FWGUI_API virtual void setTitle(const std::string& title) override;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string& msg) override;

    /// Set the input text in the input field
    FWGUI_API virtual void setInput(const std::string& text) override;

    /// Get the input text in the input field
    FWGUI_API virtual std::string getInput() override;

protected:

    /// Implementation of an input dialog in a specific IHM (wx/Qt)
    ::fwGui::dialog::IInputDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_INPUTDIALOG_HPP__*/

