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
 * @brief   Defines the generic input dialog box for UI.
 */
class SIGHT_UI_CLASS_API input_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(input_base, ui::object);

    enum class echo_mode : uint8_t
    {
        normal       = 0,
        noecho       = 1,
        password     = 2,
        echo_on_edit = 3,
        DEFAULT      = normal
    };

    /// Constructor. Do nothing.
    SIGHT_UI_API input_base();
    /// Destructor. Do nothing.
    SIGHT_UI_API ~input_base() override;

    using factory_registry_key_t = std::string;
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    /// Set the title of the input dialog
    SIGHT_UI_API virtual void set_title(const std::string& _title) = 0;

    /// Set the input text in the input field
    SIGHT_UI_API virtual void set_input(const std::string& _text) = 0;

    /// Get the input text in the input field
    SIGHT_UI_API virtual std::pair<std::string, bool> get_input() = 0;

    /// Set the message
    SIGHT_UI_API virtual void set_message(const std::string& _msg) = 0;

    /// Sets the echo mode used to display input field content
    SIGHT_UI_API virtual void set_echo_mode(echo_mode _echo_mode) = 0;
};

} // namespace sight::ui::dialog
