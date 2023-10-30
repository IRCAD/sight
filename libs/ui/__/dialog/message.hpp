/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/dialog/message_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic message box for IHM.
 * Use the Delegate design pattern.
 */
class UI_CLASS_API message : public ui::dialog::message_base
{
public:

    SIGHT_DECLARE_CLASS(message, ui::dialog::message_base);

    /**
     * Opens a default message box with the specified title, text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     * @return Returns the button enum value corresponding to the pressed button
     */
    UI_API static message::buttons show(
        const std::string& _title,
        const std::string& _message,
        message::icons _icon = info
    );

    /// Constructor. Create the implementation of the specific message box
    UI_API message();

    /**
     * Constructor
     * Creates a message box with the specified title text and icon.
     *
     * @param title Title of the message box
     * @param message Message of the message box
     * @param icon Icon of the message box (CRITICAL, WARNING, INFO or QUESTION)
     */
    UI_API message(const std::string& _title, const std::string& _message, message::icons _icon = info);

    /// Destructor. Do nothing
    UI_API ~message() override;

    /// Set the title of the message box
    UI_API void set_title(const std::string& _title) override;

    /// Set the message
    UI_API void set_message(const std::string& _msg) override;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    UI_API void set_icon(message::icons _icon) override;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL)
    UI_API void add_button(message::buttons _button) override;

    /// Set the default button
    UI_API void set_default_button(message::buttons _button) override;

    /// Add a custom button to this dialog
    UI_API void add_custom_button(const std::string& _label, std::function<void()> _clicked_fn) override;

    /// Show the message box and return the clicked button.
    UI_API message::buttons show() override;

protected:

    /// Implementation of a message box in a specific IHM (wx/Qt)
    ui::dialog::message_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
