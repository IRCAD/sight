/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_MESSAGEBOX_HPP_
#define _FWGUIQT_MESSAGEBOX_HPP_

#include <string>
#include <fwCore/base.hpp>

#include <fwGui/dialog/IMessageDialog.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   Defines the generic message box for IHM.
 * @class   MessageDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API MessageDialog : public ::fwGui::dialog::IMessageDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MessageDialog)(::fwGui::dialog::IMessageDialog), (()), new MessageDialog );

    /// Constructor. Do nothing.
    FWGUIQT_API MessageDialog();
    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~MessageDialog();

    /// Set the title of the message box
    FWGUIQT_API virtual void setTitle( const std::string &title );

    /// Set the message
    FWGUIQT_API virtual void setMessage( const std::string &msg );

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    FWGUIQT_API virtual void setIcon( IMessageDialog::Icons icon);

    /// Add a button (OK, YES_NO, CANCEL)
    FWGUIQT_API virtual void addButton( IMessageDialog::Buttons button );

    /// Show the message box and return the clicked button.
    FWGUIQT_API virtual Buttons show();

protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// List of the button
    ::fwGui::dialog::IMessageDialog::Buttons m_buttons;
    /// Icon
    ::fwGui::dialog::IMessageDialog::Icons m_icon;

    //static const std::map< ::fwGui::dialog::IMessageDialog::Icons, QMessageBox::Icon> qtIcons;
};
} // namespace dialog
} // namespace fwGuiQt

#endif /*_FWGUIQT_MESSAGEBOX_HPP_*/


