/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_INPUTDIALOG_HPP_
#define _FWGUI_INPUTDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/IInputDialog.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

/**
 * @brief   Defines the generic input dialog for IHM.
 * Use the Delegate design pattern
 * @class   InputDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API InputDialog : public ::fwGui::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::IInputDialog), (()), new InputDialog );

    /* @brief Constructor. Create the implementation of the specific input dialog box
     * Example of use:
     * @verbatinm
          ::fwGui::InputDialog inputDlg;
          inputDlg.setTitle("Identification dialog");
          inputDlg.setMessage("Enter Pin Code: ");
          inputDlg.setInput ("<Enter your code here>");
          std::string inputText = inputDlg.getInput();
     @endverbatim
     */

    FWGUI_API InputDialog();
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
    ::fwGui::IInputDialog::sptr m_implementation;
};

} // namespace fwGui

#endif /*_FWGUI_INPUTDIALOG_HPP_*/


