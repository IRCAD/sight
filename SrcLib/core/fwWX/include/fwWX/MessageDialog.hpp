/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_MESSAGEBOX_HPP_
#define _FWWX_MESSAGEBOX_HPP_

#include <string>
#include <fwCore/base.hpp>



#include <fwGui/IMessageDialog.hpp>

#include "fwWX/config.hpp"

namespace fwWX
{

/**
 * @brief   Defines the generic message box for IHM.
 * @class   MessageDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWWX_CLASS_API MessageDialog : public ::fwGui::IMessageDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MessageDialog)(::fwGui::IMessageDialog), (()), new MessageDialog );

    /// Constructor. Do nothing.
    FWWX_API MessageDialog();
    /// Destructor. Do nothing.
    FWWX_API virtual ~MessageDialog();

    /// Set the title of the message box
    FWWX_API virtual void setTitle( const std::string &title );

    /// Set the message
    FWWX_API virtual void setMessage( const std::string &msg );

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    FWWX_API virtual void setIcon( IMessageDialog::Icons icon);

    /// Add a button (OK, YES_NO, CANCEL)
    FWWX_API virtual void addButton( IMessageDialog::Buttons button );

    /// Show the message box and return the clicked button.
    FWWX_API virtual Buttons show();

protected:
    /// Dialog title
    std::string m_title;
    /// Dialog box message
    std::string m_message;
    /// List of the button in wx style (wxOK, wxCANCEL, wxYES_NO)
    unsigned long m_buttons;
    /// Icon in wx style (wxICON_ERROR, wxICON_INFORMATION, ...)
    unsigned long m_icon;
};

} // namespace fwWX

#endif /*_FWWX_MESSAGEBOX_HPP_*/


