/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_INPUTDIALOG_HPP_
#define _FWGUIQT_INPUTDIALOG_HPP_

#include <string>
#include <fwCore/base.hpp>

#include <fwGui/dialog/IInputDialog.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   Defines an Input dialog.
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
class FWGUIQT_CLASS_API InputDialog : public ::fwGui::dialog::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::dialog::IInputDialog), (()), new InputDialog );

    /// Constructor. Do nothing.
    FWGUIQT_API InputDialog();
    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~InputDialog();

    /// Set the title of the message box
    FWGUIQT_API virtual void setTitle( const std::string &title );

    /// Set the message
    FWGUIQT_API virtual void setMessage( const std::string &msg );

    /// Set the input text in the input field
    FWGUIQT_API virtual void setInput(const std::string &text);

    /// Get the input text in the input field
    FWGUIQT_API virtual std::string getInput();

protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// Text inputed
    std::string m_input;

};
} // namespace dialog
} // namespace fwGuiQt

#endif /*_FWGUIQT_INPUTDIALOG_HPP_*/


