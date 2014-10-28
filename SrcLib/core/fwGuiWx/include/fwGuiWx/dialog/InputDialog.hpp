/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_INPUTDIALOG_HPP_
#define _FWGUIWX_INPUTDIALOG_HPP_

#include <string>
#include <fwCore/base.hpp>

#include <fwGui/dialog/IInputDialog.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace dialog
{
/**
 * @brief   Defines an input dialog for IHM on WxWidget.
 * @class   InputDialog
 * 
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API InputDialog : public ::fwGui::dialog::IInputDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (InputDialog)(::fwGui::dialog::IInputDialog),
                                            (()),
                                            ::fwGui::factory::New< InputDialog > );

    /** @brief Constructor. Do nothing.\n
     * Example of use: \n
     * @verbatim
          ::fwGui::dialog::InputDialog inputDlg;
          inputDlg.setTitle("Identification dialog");
          inputDlg.setMessage("Enter Pin Code: ");
          inputDlg.setInput ("<Enter your code here>");
          std::string inputText = inputDlg.getInput();
     @endverbatim
     */

    FWGUIWX_API InputDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIWX_API virtual ~InputDialog();

    /// Set the title of the message box.
    FWGUIWX_API virtual void setTitle( const std::string &title );

    /// Set the message
    FWGUIWX_API virtual void setMessage( const std::string &msg );

    /// Set the input text in the input field.
    FWGUIWX_API virtual void setInput(const std::string &text);

    /// Get the input text in the input field
    FWGUIWX_API virtual std::string getInput();


protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// Text inputed
    std::string m_input;

};

} // namespace dialog
} // namespace fwGuiWx

#endif /*_FWGUIWX_INPUTDIALOG_HPP_*/


