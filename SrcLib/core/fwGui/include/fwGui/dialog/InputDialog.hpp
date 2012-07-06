/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_INPUTDIALOG_HPP_
#define _FWGUI_INPUTDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/dialog/IInputDialog.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic input dialog for IHM.
 * Use the Delegate design pattern
 * @class   InputDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 * Example of use:
 * @verbatim
  ::fwGui::dialog::InputDialog inputDlg;
  inputDlg.setTitle("Identification dialog");
  inputDlg.setMessage("Enter Pin Code: ");
  inputDlg.setInput ("<Enter your code here>");
  std::string inputText = inputDlg.getInput();
  @endverbatim
 */
class FWGUI_CLASS_API InputDialog : public ::fwGui::dialog::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::dialog::IInputDialog), (()), new InputDialog );

    /**
     * Opens a default input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     * @return Returns text in the input field
     */
    FWGUI_API static std::string showInputDialog(const std::string& title, const std::string& message, const std::string &text = "");


    /** @brief Constructor. Create the implementation of the specific input dialog box.
     *
     * Example of use:
     * @verbatim
      ::fwGui::dialog::InputDialog inputDlg;
      inputDlg.setTitle("Identification dialog");
      inputDlg.setMessage("Enter Pin Code: ");
      inputDlg.setInput ("<Enter your code here>");
      std::string inputText = inputDlg.getInput();
     @endverbatim
     */
    FWGUI_API InputDialog();

    /**
     * Constructor. Creates a input box with the specified title and message.
     *
     * @param title Title of the input box
     * @param message Message of the input box
     * @param text the input text in the input field
     */
    FWGUI_API InputDialog(const std::string& title, const std::string& message, const std::string &text = "");

    /// Destructor. Do nothing
    FWGUI_API virtual ~InputDialog();

    /// Set the title of the input dialog
    FWGUI_API virtual void setTitle(const std::string &title);

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg);

    /// Set the input text in the input field
    FWGUI_API virtual void setInput(const std::string &text);

    /// Get the input text in the input field
    FWGUI_API virtual std::string getInput();

protected:

    /// Implementation of an input dialog in a specific IHM (wx/Qt)
    ::fwGui::dialog::IInputDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_INPUTDIALOG_HPP_*/


