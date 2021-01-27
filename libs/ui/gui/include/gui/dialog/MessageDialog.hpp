/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "gui/dialog/IMessageDialog.hpp"

#include <core/base.hpp>

namespace sight::gui
{
namespace dialog
{
/**
 * @brief   Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 */
class GUI_CLASS_API MessageDialog : public gui::dialog::IMessageDialog
{

public:

    fwCoreClassMacro(MessageDialog, gui::dialog::IMessageDialog, new MessageDialog)

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     * @return Returns the button enum value corresponding to the pressed button
     */
    GUI_API static IMessageDialog::Buttons show(
        const std::string& title, const std::string& message, gui::dialog::IMessageDialog::Icons icon = INFO);

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     * @deprecated use show() instead
     * @return Returns the button enum value corresponding to the pressed button
     */
    [[deprecated("Sight 22.0, use show() instead.")]]
    GUI_API static IMessageDialog::Buttons showMessageDialog(
        const std::string& title, const std::string& message, gui::dialog::IMessageDialog::Icons icon = INFO);

    /// Constructor. Create the implementation of the specific message box
    GUI_API MessageDialog();

    /**
     * Constructor
     * Creates a message box with the specified title text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     */
    GUI_API MessageDialog(
        const std::string& title, const std::string& message, gui::dialog::IMessageDialog::Icons icon = INFO);

    /// Destructor. Do nothing
    GUI_API virtual ~MessageDialog();

    /// Set the title of the message box
    GUI_API virtual void setTitle(const std::string& title) override;

    /// Set the message
    GUI_API virtual void setMessage(const std::string& msg) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    GUI_API virtual void setIcon( IMessageDialog::Icons icon) override;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL)
    GUI_API virtual void addButton(IMessageDialog::Buttons button ) override;

    /// Set the default button
    GUI_API virtual void setDefaultButton(IMessageDialog::Buttons button ) override;

    /// Add a custom button to this dialog
    GUI_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    /// Show the message box and return the clicked button.
    GUI_API virtual IMessageDialog::Buttons show() override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    gui::dialog::IMessageDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace sight::gui
