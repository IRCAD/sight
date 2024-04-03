/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/ui/__/config.hpp>

#include "ui/__/object.hpp"

#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic message box for IHM.
 */
class SIGHT_UI_CLASS_API message_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(message_base, ui::object);

    /// Icon type
    enum icons
    {
        critical,
        warning,
        info,
        question,
        none
    };

    /// Buttons type
    enum buttons
    {
        nobutton = 0, // invalid

        ok     = 1 << 1,
        yes    = 1 << 2,
        no     = 1 << 3,
        cancel = 1 << 4,
        retry  = 1 << 5,
        yes_no = yes | no
    };

    /// Constructor. Do nothing.
    SIGHT_UI_API message_base();
    /// Destructor. Do nothing.
    SIGHT_UI_API ~message_base() override;

    using factory_registry_key_t = std::string;
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    /// Set the title of the message box
    SIGHT_UI_API virtual void set_title(const std::string& _title) = 0;

    /// Set the message
    SIGHT_UI_API virtual void set_message(const std::string& _msg) = 0;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    SIGHT_UI_API virtual void set_icon(icons _icon) = 0;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL, RETRY)
    SIGHT_UI_API virtual void add_button(buttons _button) = 0;

    /// Set the default button
    SIGHT_UI_API virtual void set_default_button(buttons _button) = 0;

    /// Add a custom button to this dialog
    SIGHT_UI_API virtual void add_custom_button(const std::string& _label, std::function<void()> _clicked_fn) = 0;

    /// Show the message box and return the clicked button.
    SIGHT_UI_API virtual buttons show() = 0;
};

} // namespace sight::ui::dialog
