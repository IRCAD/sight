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

#include "ui/base/config.hpp"
#include "ui/base/dialog/IMessageDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{
namespace dialog
{
/**
 * @brief   Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 */
class UI_BASE_CLASS_API MessageDialog : public ui::base::dialog::IMessageDialog
{

public:

    SIGHT_DECLARE_CLASS(MessageDialog, ui::base::dialog::IMessageDialog, new MessageDialog)

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     * @return Returns the button enum value corresponding to the pressed button
     */
    UI_BASE_API static IMessageDialog::Buttons show(
        const std::string& title, const std::string& message, ui::base::dialog::IMessageDialog::Icons icon = INFO);

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
    UI_BASE_API static IMessageDialog::Buttons showMessageDialog(
        const std::string& title, const std::string& message, ui::base::dialog::IMessageDialog::Icons icon = INFO);

    /// Constructor. Create the implementation of the specific message box
    UI_BASE_API MessageDialog();

    /**
     * Constructor
     * Creates a message box with the specified title text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     */
    UI_BASE_API MessageDialog(
        const std::string& title, const std::string& message, ui::base::dialog::IMessageDialog::Icons icon = INFO);

    /// Destructor. Do nothing
    UI_BASE_API virtual ~MessageDialog();

    /// Set the title of the message box
    UI_BASE_API virtual void setTitle(const std::string& title) override;

    /// Set the message
    UI_BASE_API virtual void setMessage(const std::string& msg) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    UI_BASE_API virtual void setIcon( IMessageDialog::Icons icon) override;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL)
    UI_BASE_API virtual void addButton(IMessageDialog::Buttons button ) override;

    /// Set the default button
    UI_BASE_API virtual void setDefaultButton(IMessageDialog::Buttons button ) override;

    /// Add a custom button to this dialog
    UI_BASE_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

    /// Show the message box and return the clicked button.
    UI_BASE_API virtual IMessageDialog::Buttons show() override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ui::base::dialog::IMessageDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace sight::ui::base
