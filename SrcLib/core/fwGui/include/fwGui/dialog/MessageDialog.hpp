/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_MESSAGEBOX_HPP_
#define _FWGUI_MESSAGEBOX_HPP_

#include <fwCore/base.hpp>

#include "fwGui/dialog/IMessageDialog.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 * @class   MessageDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API MessageDialog : public ::fwGui::dialog::IMessageDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MessageDialog)(::fwGui::dialog::IMessageDialog), (()), new MessageDialog );

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     * @return Returns the button enum value corresponding to the pressed button
     */
    FWGUI_API static IMessageDialog::Buttons showMessageDialog(const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon = INFO);

    /// Constructor. Create the implementation of the specific message box
    FWGUI_API MessageDialog();

    /**
     * Constructor
     * Creates a message box with the specified title text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     */
    FWGUI_API MessageDialog(const std::string& title, const std::string& message, ::fwGui::dialog::IMessageDialog::Icons icon = INFO);

    /// Destructor. Do nothing
    FWGUI_API virtual ~MessageDialog();

    /// Set the title of the message box
    FWGUI_API virtual void setTitle(const std::string &title);

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg);

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    FWGUI_API virtual void setIcon( IMessageDialog::Icons icon);

    /// Add a button (OK, YES_NO, YES, NO, CANCEL)
    FWGUI_API virtual void addButton(IMessageDialog::Buttons button );

    /// Show the message box and return the clicked button.
    FWGUI_API virtual IMessageDialog::Buttons show();

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ::fwGui::dialog::IMessageDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_MESSAGEBOX_HPP_*/


